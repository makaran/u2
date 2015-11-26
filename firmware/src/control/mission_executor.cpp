#include "main.h"

#include "mission_executor.hpp"
#include "waypoint_db.hpp"
#include "mav_dbg.hpp"
#include "navigator_types.hpp"
#include "time_keeper.hpp"
#include "mav_logger.hpp"
#include "mav_postman.hpp"

using namespace chibios_rt;
using namespace control;

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

#define MSN_EXEC_DEBUG_VECT_DECIMATOR 10
// some convenient aliases
#define WP_RADIUS     param2

#define JUMP_REPEAT   param2
#define JUMP_SEQ      param1

#define PID_TUNE_DEBUG      TRUE

#define MISSION_EXECUTOR_DEBUG TRUE

/*
 ******************************************************************************
 * EXTERNS
 ******************************************************************************
 */
extern mavlink_system_t                 mavlink_system_struct;
extern mavlink_mission_current_t        mavlink_out_mission_current_struct;
extern mavlink_mission_item_reached_t   mavlink_out_mission_item_reached_struct;
extern mavlink_nav_controller_output_t  mavlink_out_nav_controller_output_struct;

extern EvtSource event_mission_reached;

extern MavLogger mav_logger;

/*
 ******************************************************************************
 * PROTOTYPES
 ******************************************************************************
 */

/*
 ******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************
 */

#if PID_TUNE_DEBUG
__CCM__ static mavMail pid_tune_mail;
__CCM__ static mavlink_debug_vect_t mavlink_out_debug_vect_struct = {0, 0, 0, 0, "PID_TUNE"};
#endif

#if MISSION_EXECUTOR_DEBUG
__CCM__ static mavlink_debug_vect_t dbg_msn_exec;
__CCM__ static mavMail mail_msn_exec;
#endif

/*
 ******************************************************************************
 ******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************
 ******************************************************************************
 */
/**
 *
 */
void MissionExecutor::broadcast_mission_current(uint16_t seq) {

  mavlink_out_mission_current_struct.seq = seq;
}

/**
 *
 */
void MissionExecutor::broadcast_mission_item_reached(uint16_t seq) {

  event_mission_reached.broadcastFlags(EVMSK_MISSION_REACHED | (seq << 16));
  mavlink_out_mission_item_reached_struct.seq = seq;
}

/**
 *
 */
uint16_t MissionExecutor::jump_to_handler(uint16_t next) {

  /* protection from idiots */
  if (0 == round(third.JUMP_REPEAT))
    return next+1;

  /**/
  if (! jumpctx.active) {
    jumpctx.active = true;
    jumpctx.remain = round(third.JUMP_REPEAT);
  }
  else {
    jumpctx.remain--;
    if (0 == jumpctx.remain) {
      jumpctx.active = false;
      return next+1;
    }
  }

  return round(third.JUMP_SEQ);
}

/**
 *
 */
bool MissionExecutor::load_next_mission_item(void) {

  bool load_status = OSAL_FAILED;
  uint16_t next;

  prev = trgt;
  trgt = third;

  if (wpdb.getCount() == (third.seq + 1)) {
    /* if we fall here than last mission was not 'land'. System do not know
     * what to do so start unlimited loitering */
    third = trgt;
    third.x += 0.0001; /* singularity prevention */
    third.y += 0.0001; /* singularity prevention */
    third.command = MAV_CMD_NAV_LOITER_UNLIM;
    third.seq += 1;
    broadcast_mission_current(trgt.seq);
  }
  else if (wpdb.getCount() < (third.seq + 1)) {
    state = MissionState::completed;
    load_status = OSAL_SUCCESS;
  }
  else {
    next = third.seq + 1;
    __LOAD_JUMP:
    load_status = wpdb.read(&third, next);
    if (OSAL_SUCCESS == load_status) {
      /* handle 'jump_to' command */
      if (MAV_CMD_DO_JUMP == third.command) {
        next = jump_to_handler(next);
        goto __LOAD_JUMP;
      }
      else {
        state = MissionState::navigate;
        broadcast_mission_current(trgt.seq);
      }
    }
    else {
      state = MissionState::error;
    }
  }

  return load_status;
}

#ifndef USE_LD_NAVIGATOR
/**
 *
 */
void MissionExecutor::navout2acsin(const NavOut<double> &nav_out) {
  acs_in.ch[ACS_INPUT_dZrad] = nav_out.xtd;
  acs_in.ch[ACS_INPUT_dZm]   = rad2m(nav_out.xtd);

  float dYaw = acs_in.ch[ACS_INPUT_yaw] - static_cast<float>(navigator.get_crsAB());
  acs_in.ch[ACS_INPUT_dYaw] = wrap_pi(dYaw);
}

/**
 *
 */
void MissionExecutor::navout2mavlink(const NavOut<double> &nav_out) {

  mavlink_out_nav_controller_output_struct.wp_dist = rad2m(nav_out.dist);
  mavlink_out_nav_controller_output_struct.xtrack_error = rad2m(nav_out.xtd);
  mavlink_out_nav_controller_output_struct.target_bearing = rad2deg(nav_out.crs);
  mavlink_out_nav_controller_output_struct.nav_bearing = rad2deg(acs_in.ch[ACS_INPUT_dYaw]);

  mavlink_out_debug_vect_struct.x = mavlink_out_nav_controller_output_struct.xtrack_error;
  mavlink_out_debug_vect_struct.y = mavlink_out_nav_controller_output_struct.target_bearing;
  mavlink_out_debug_vect_struct.z = mavlink_out_nav_controller_output_struct.nav_bearing;
  mavlink_out_debug_vect_struct.time_usec = TIME_BOOT_MS;

#if PID_TUNE_DEBUG
  if (pid_tune_mail.free()) {
    pid_tune_mail.fill(&mavlink_out_debug_vect_struct,
        MAV_COMP_ID_ALL, MAVLINK_MSG_ID_DEBUG_VECT);
    mav_logger.write(&pid_tune_mail);
  }
#endif
}

/**
 * @brief   Detects waypoint reachable status.
 * @details Kind of hack. It increases effective radius if
 *          crosstrack error more than R/1.5
 */
bool MissionExecutor::wp_reached(const NavOut<double> &nav_out) {

  double Rmeters = rad2m(nav_out.xtd);
  Rmeters = fabs(Rmeters * static_cast<double>(1.5));

  if (Rmeters < static_cast<double>(trgt.WP_RADIUS))
    Rmeters = trgt.WP_RADIUS;
  /* else branch unneeded here because Rmeters already contains correct value */

  return rad2m(nav_out.dist) < Rmeters;
}

#else

void MissionExecutor::navout2acsin(const LdNavOut<double> &nav_out) {
  acs_in.ch[ACS_INPUT_dZm] = static_cast<float>(nav_out.dz);

  float dYaw = acs_in.ch[ACS_INPUT_yaw] - static_cast<float>(nav_out.crs);
  acs_in.ch[ACS_INPUT_dYaw] = wrap_pi(dYaw);
}

void MissionExecutor::navout2mavlink(const LdNavOut<double> &nav_out) {

  mavlink_out_nav_controller_output_struct.wp_dist = static_cast<uint16_t>(round(nav_out.dist));
  mavlink_out_nav_controller_output_struct.xtrack_error = static_cast<float>(nav_out.dz);
  mavlink_out_nav_controller_output_struct.target_bearing = rad2deg(nav_out.crs);
  mavlink_out_nav_controller_output_struct.nav_bearing = rad2deg(acs_in.ch[ACS_INPUT_dYaw]);

#if PID_TUNE_DEBUG
  mavlink_out_debug_vect_struct.x = mavlink_out_nav_controller_output_struct.xtrack_error;
  mavlink_out_debug_vect_struct.y = mavlink_out_nav_controller_output_struct.target_bearing;
  mavlink_out_debug_vect_struct.z = mavlink_out_nav_controller_output_struct.nav_bearing;
  mavlink_out_debug_vect_struct.time_usec = TIME_BOOT_MS;
  if (pid_tune_mail.free()) {
    pid_tune_mail.fill(&mavlink_out_debug_vect_struct,
        MAV_COMP_ID_ALL, MAVLINK_MSG_ID_DEBUG_VECT);
    mav_logger.write(&pid_tune_mail);
  }
#endif
}

void MissionExecutor::debug2mavlink() {
#if MISSION_EXECUTOR_DEBUG

  uint64_t time = TimeKeeper::utc();
  dbg_msn_exec.time_usec = time;
//  dbg_msn_exec.x = static_cast<float>(mnr_parser.debugPartNumber());
  dbg_msn_exec.x = static_cast<float>(mnr_executor.debugPartNumber());
  dbg_msn_exec.y = acs_in.ch[ACS_INPUT_dYaw];
  dbg_msn_exec.z = acs_in.ch[ACS_INPUT_dZm];

  mail_msn_exec.fill(&dbg_msn_exec, MAV_COMP_ID_SYSTEM_CONTROL, MAVLINK_MSG_ID_DEBUG_VECT);
  mav_postman.post(mail_msn_exec);

#endif
}

bool MissionExecutor::wp_reached(const LdNavOut<double> &nav_out,
                                 const MnrPart<double> &part) {
  return nav_out.crossed && part.finale;
}

bool MissionExecutor::mnr_part_reached(const LdNavOut<double> &nav_out) {
  return nav_out.crossed;
}

#endif
/**
 *
 */
void MissionExecutor::navigate(void) {
#ifndef USE_LD_NAVIGATOR
  NavIn<double> nav_in(deg2rad(acs_in.ch[ACS_INPUT_lat]),
                       deg2rad(acs_in.ch[ACS_INPUT_lon]));
  NavOut<double> nav_out = navigator.update(nav_in);
  navout2acsin(nav_out);
  navout2mavlink(nav_out);
  if (wp_reached(nav_out)) {
    broadcast_mission_item_reached(trgt.seq);
    load_next_mission_item();
    NavLine<double> line(deg2rad(prev.x), deg2rad(prev.y), deg2rad(trgt.x), deg2rad(trgt.y));
    navigator.loadLine(line);
  }
#else
  double curr_wgs84[3][1] = {{deg2rad(acs_in.chd[ACS_DOUBLE_INPUT_lat])},
                             {deg2rad(acs_in.chd[ACS_DOUBLE_INPUT_lon])},
                             {static_cast<double>(acs_in.ch[ACS_INPUT_alt])}};
//  MnrPart<double> part = mnr_parser.update(curr_wgs84);
//  LdNavOut<double> nav_out = ld_navigator.update(part);
  LdNavOut<double> nav_out = mnr_executor.update(curr_wgs84);
  navout2acsin(nav_out);
  navout2mavlink(nav_out);

#if MISSION_EXECUTOR_DEBUG
  if (0 == send_debug_vect_decimator)
    debug2mavlink();
  send_debug_vect_decimator++;
  if (send_debug_vect_decimator >= MSN_EXEC_DEBUG_VECT_DECIMATOR)
    send_debug_vect_decimator = 0;
#endif

//  if (wp_reached(nav_out, part)) {
//    broadcast_mission_item_reached(trgt.seq);
//    load_next_mission_item();
//    mnr_parser.resetPartCounter();
//  } else if (mnr_part_reached(nav_out)) {
//    mnr_parser.loadNextPart();
//  }

  if (mnr_executor.isManeuverCompleted()) {
    broadcast_mission_item_reached(trgt.seq);
    load_next_mission_item();
  }

#endif
}

/*
 ******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************
 */

/**
 *
 */
MissionExecutor::MissionExecutor(ACSInput &acs_in) :
  state(MissionState::uninit),
  acs_in(acs_in),
//  mnr_parser(prev, trgt, third),
  mnr_executor(prev, trgt, third),
  send_debug_vect_decimator(0) {

  /* fill home point with invalid data. This denotes it uninitialized. */
  memset(&home, 0xFF, sizeof(home));
  home.seq = -1;

#if MISSION_EXECUTOR_DEBUG
  const size_t N = sizeof(mavlink_debug_vect_t::name);
  strncpy(dbg_msn_exec.name,  "msn_exec",  N);
#endif

}

/**
 *
 */
void MissionExecutor::start(void) {
  chTMObjectInit(&this->tmp_nav);
  state = MissionState::idle;

}

/**
 *
 */
void MissionExecutor::stop(void) {
  state = MissionState::uninit;
}

/**
 *
 */
void MissionExecutor::artificial_takeoff_point(void) {

  memset(&prev, 0, sizeof(prev));

  prev.x = acs_in.ch[ACS_INPUT_lat];
  prev.y = acs_in.ch[ACS_INPUT_lon];
  prev.z = acs_in.ch[ACS_INPUT_alt];
  prev.command = MAV_CMD_NAV_TAKEOFF;
  prev.frame = MAV_FRAME_GLOBAL;
  prev.seq = -1;

  /* store launch point */
  launch = prev;

  /* set launch point as home if it was not set yet from ground */
  if (uint16_t(-1) == home.seq) {
    home = launch;
    home.command = MAV_CMD_NAV_WAYPOINT;
  }
}


/**
 * @brief   Samples current coordinates as prev WP than append
 *          first 2 points from mission.
 */
bool MissionExecutor::takeoff(void) {
  bool read_status1 = OSAL_FAILED;
  bool read_status2 = OSAL_FAILED;

  if (wpdb.getCount() < 2) {
    mavlink_dbg_print(MAV_SEVERITY_INFO, "ACS: mission must be at least 2 WP long", MAV_COMP_ID_SYSTEM_CONTROL);
    return OSAL_FAILED;
  }
  else {
    artificial_takeoff_point();
    read_status1 = wpdb.read(&this->trgt,  0);
    read_status2 = wpdb.read(&this->third, 1);
    if ((OSAL_SUCCESS != read_status1) || (OSAL_SUCCESS != read_status2))
      return OSAL_FAILED;
    else {
#ifndef USE_LD_NAVIGATOR
      NavLine<double> line(deg2rad(prev.x), deg2rad(prev.y), deg2rad(trgt.x), deg2rad(trgt.y));
      navigator.loadLine(line);
#endif
      state = MissionState::navigate;
      return OSAL_SUCCESS;
    }
  }
}

/**
 *
 */
MissionState MissionExecutor::update(void) {

  osalDbgCheck(MissionState::uninit != state);

  switch (state) {
  case MissionState::navigate:
    chTMStartMeasurementX(&this->tmp_nav);
    this->navigate();
    chTMStopMeasurementX(&this->tmp_nav);
    break;

  default:
    break;
  }

  return this->state;
}

/**
 *
 */
void MissionExecutor::setHome(void) {

  setHome(acs_in.ch[ACS_INPUT_lat], acs_in.ch[ACS_INPUT_lon], acs_in.ch[ACS_INPUT_alt]);
}

/**
 * @brief   Accepts coordinates in radians
 */
void MissionExecutor::setHome(float lat, float lon, float alt) {

  memset(&home, 0, sizeof(home));

  home.x = lat;
  home.y = lon;
  home.z = alt;
  home.command = MAV_CMD_NAV_WAYPOINT;
  home.frame = MAV_FRAME_GLOBAL;
  home.seq = -1;
}

/**
 *
 */
void MissionExecutor::forceGoHome(void) {
  return;
}

/**
 *
 */
uint16_t MissionExecutor::getCurrentMission(void) const {
  return this->trgt.seq;
}

/**
 *
 */
void MissionExecutor::forceReturnToLaunch(void) {
  return;
}

/**
 *
 */
bool MissionExecutor::forceJumpTo(uint16_t seq) {
  (void)seq;

  return OSAL_FAILED;
}
