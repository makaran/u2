/* 
!!! Automatically generated by
src/mavlink_local/gen_tlm_sender.py
Do not edit it manually. 
*/
#include "main.h"
#include "mavlink_local.hpp"
#include "param_registry.hpp"
#include "global_flags.h"
#include "tlm_sender.hpp"
#include "mav_mail.hpp"
#include "mav_postman.hpp"

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

/*
 ******************************************************************************
 * EXTERNS
 ******************************************************************************
 */
extern const mavlink_attitude_t mavlink_out_attitude_struct;
extern const mavlink_debug_t mavlink_out_debug_struct;
extern const mavlink_debug_vect_t mavlink_out_debug_vect_struct;
extern const mavlink_global_position_int_t mavlink_out_global_position_int_struct;
extern const mavlink_gps_raw_int_t mavlink_out_gps_raw_int_struct;
extern const mavlink_highres_imu_t mavlink_out_highres_imu_struct;
extern const mavlink_mission_current_t mavlink_out_mission_current_struct;
extern const mavlink_nav_controller_output_t mavlink_out_nav_controller_output_struct;
extern const mavlink_local_position_ned_t mavlink_out_local_position_ned_struct;
extern const mavlink_raw_imu_t mavlink_out_raw_imu_struct;
extern const mavlink_raw_pressure_t mavlink_out_raw_pressure_struct;
extern const mavlink_rc_channels_t mavlink_out_rc_channels_struct;
extern const mavlink_rc_channels_scaled_t mavlink_out_rc_channels_scaled_struct;
extern const mavlink_scaled_pressure_t mavlink_out_scaled_pressure_struct;
extern const mavlink_sys_status_t mavlink_out_sys_status_struct;
extern const mavlink_vfr_hud_t mavlink_out_vfr_hud_struct;


/*
 ******************************************************************************
 * PROTOTYPES
 ******************************************************************************
 */
/* sending function */
typedef void (*send_t)(void);

struct tlm_registry_t {
  tlm_registry_t(systime_t nd, uint32_t const *sp, const send_t s) :
    next_dealine(nd), sleepperiod(sp), sender(s){};
  tlm_registry_t(void) = delete;
  /* how much to sleep */
  systime_t next_dealine;
  /* pointer to period value in global parameters structure */
  uint32_t const *sleepperiod;
  /* sending function */
  const send_t sender;
};

static void send_attitude(void);
static void send_debug(void);
static void send_debug_vect(void);
static void send_global_pos(void);
static void send_gps_raw_int(void);
static void send_highres_imu(void);
static void send_mission_curr(void);
static void send_nav_output(void);
static void send_position_ned(void);
static void send_raw_imu(void);
static void send_raw_press(void);
static void send_rc(void);
static void send_rc_scaled(void);
static void send_scal_press(void);
static void send_sys_status(void);
static void send_vfr_hud(void);

/*
 ******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************
 */

static uint32_t mailbox_overflow = 0;
static uint32_t mail_undelivered = 0;
static bool pause_flag = false;

static mavMail attitude_mail __CCM__;
static mavMail debug_mail __CCM__;
static mavMail debug_vect_mail __CCM__;
static mavMail global_position_int_mail __CCM__;
static mavMail gps_raw_int_mail __CCM__;
static mavMail highres_imu_mail __CCM__;
static mavMail mission_current_mail __CCM__;
static mavMail nav_controller_output_mail __CCM__;
static mavMail local_position_ned_mail __CCM__;
static mavMail raw_imu_mail __CCM__;
static mavMail raw_pressure_mail __CCM__;
static mavMail rc_channels_mail __CCM__;
static mavMail rc_channels_scaled_mail __CCM__;
static mavMail scaled_pressure_mail __CCM__;
static mavMail sys_status_mail __CCM__;
static mavMail vfr_hud_mail __CCM__;

/* autoinitialized array */
__CCM__ static tlm_registry_t Registry[] = {
    {11, nullptr, send_attitude},
    {12, nullptr, send_debug},
    {13, nullptr, send_debug_vect},
    {14, nullptr, send_global_pos},
    {15, nullptr, send_gps_raw_int},
    {16, nullptr, send_highres_imu},
    {17, nullptr, send_mission_curr},
    {18, nullptr, send_nav_output},
    {19, nullptr, send_position_ned},
    {20, nullptr, send_raw_imu},
    {21, nullptr, send_raw_press},
    {22, nullptr, send_rc},
    {23, nullptr, send_rc_scaled},
    {24, nullptr, send_scal_press},
    {25, nullptr, send_sys_status},
    {26, nullptr, send_vfr_hud},
};

/*
 *******************************************************************************
 *******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************
 *******************************************************************************
 */
static void send_attitude(void){
  msg_t status = MSG_RESET;
  if (attitude_mail.free()){
    attitude_mail.fill(&mavlink_out_attitude_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_ATTITUDE);
    status = mav_postman.post(attitude_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      attitude_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_debug(void){
  msg_t status = MSG_RESET;
  if (debug_mail.free()){
    debug_mail.fill(&mavlink_out_debug_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_DEBUG);
    status = mav_postman.post(debug_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      debug_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_debug_vect(void){
  msg_t status = MSG_RESET;
  if (debug_vect_mail.free()){
    debug_vect_mail.fill(&mavlink_out_debug_vect_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_DEBUG_VECT);
    status = mav_postman.post(debug_vect_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      debug_vect_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_global_pos(void){
  msg_t status = MSG_RESET;
  if (global_position_int_mail.free()){
    global_position_int_mail.fill(&mavlink_out_global_position_int_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_GLOBAL_POSITION_INT);
    status = mav_postman.post(global_position_int_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      global_position_int_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_gps_raw_int(void){
  msg_t status = MSG_RESET;
  if (gps_raw_int_mail.free()){
    gps_raw_int_mail.fill(&mavlink_out_gps_raw_int_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_GPS_RAW_INT);
    status = mav_postman.post(gps_raw_int_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      gps_raw_int_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_highres_imu(void){
  msg_t status = MSG_RESET;
  if (highres_imu_mail.free()){
    highres_imu_mail.fill(&mavlink_out_highres_imu_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_HIGHRES_IMU);
    status = mav_postman.post(highres_imu_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      highres_imu_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_mission_curr(void){
  msg_t status = MSG_RESET;
  if (mission_current_mail.free()){
    mission_current_mail.fill(&mavlink_out_mission_current_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_MISSION_CURRENT);
    status = mav_postman.post(mission_current_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      mission_current_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_nav_output(void){
  msg_t status = MSG_RESET;
  if (nav_controller_output_mail.free()){
    nav_controller_output_mail.fill(&mavlink_out_nav_controller_output_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT);
    status = mav_postman.post(nav_controller_output_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      nav_controller_output_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_position_ned(void){
  msg_t status = MSG_RESET;
  if (local_position_ned_mail.free()){
    local_position_ned_mail.fill(&mavlink_out_local_position_ned_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_LOCAL_POSITION_NED);
    status = mav_postman.post(local_position_ned_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      local_position_ned_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_raw_imu(void){
  msg_t status = MSG_RESET;
  if (raw_imu_mail.free()){
    raw_imu_mail.fill(&mavlink_out_raw_imu_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_RAW_IMU);
    status = mav_postman.post(raw_imu_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      raw_imu_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_raw_press(void){
  msg_t status = MSG_RESET;
  if (raw_pressure_mail.free()){
    raw_pressure_mail.fill(&mavlink_out_raw_pressure_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_RAW_PRESSURE);
    status = mav_postman.post(raw_pressure_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      raw_pressure_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_rc(void){
  msg_t status = MSG_RESET;
  if (rc_channels_mail.free()){
    rc_channels_mail.fill(&mavlink_out_rc_channels_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_RC_CHANNELS);
    status = mav_postman.post(rc_channels_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      rc_channels_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_rc_scaled(void){
  msg_t status = MSG_RESET;
  if (rc_channels_scaled_mail.free()){
    rc_channels_scaled_mail.fill(&mavlink_out_rc_channels_scaled_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_RC_CHANNELS_SCALED);
    status = mav_postman.post(rc_channels_scaled_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      rc_channels_scaled_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_scal_press(void){
  msg_t status = MSG_RESET;
  if (scaled_pressure_mail.free()){
    scaled_pressure_mail.fill(&mavlink_out_scaled_pressure_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_SCALED_PRESSURE);
    status = mav_postman.post(scaled_pressure_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      scaled_pressure_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_sys_status(void){
  msg_t status = MSG_RESET;
  if (sys_status_mail.free()){
    sys_status_mail.fill(&mavlink_out_sys_status_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_SYS_STATUS);
    status = mav_postman.post(sys_status_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      sys_status_mail.release();
    }
  }
  else
    mail_undelivered++;
}

static void send_vfr_hud(void){
  msg_t status = MSG_RESET;
  if (vfr_hud_mail.free()){
    vfr_hud_mail.fill(&mavlink_out_vfr_hud_struct, MAV_COMP_ID_ALL, MAVLINK_MSG_ID_VFR_HUD);
    status = mav_postman.post(vfr_hud_mail);
    if (status != MSG_OK){
      mailbox_overflow++;
      vfr_hud_mail.release();
    }
  }
  else
    mail_undelivered++;
}


/**
 *
 */
static systime_t get_sleep_time(tlm_registry_t *R, size_t len){
  systime_t t;
  uint32_t i;

  t = R[0].next_dealine; /* just take first available */
  i = 0;
  while (i < len){
    /* determine minimum sleep time */
    if (t > R[i].next_dealine)
      t = R[i].next_dealine;
    i++;
  }
  return t;
}

/**
 * refresh deadlines according sleeped time
 */
void refresh_deadlines(tlm_registry_t *R, size_t len, systime_t t){
  uint32_t i = 0;
  while (i < len){
    R[i].next_dealine -= t;
    if (R[i].next_dealine == 0){
      if (*(R[i].sleepperiod) != TELEMETRY_SEND_OFF){
        R[i].next_dealine = *(R[i].sleepperiod);
        R[i].sender();
      }
      else
        R[i].next_dealine = 1200;
    }
    i++;
  }
}

/**
 * Listen events with new parameters
 */
static THD_WORKING_AREA(TlmSenderThreadWA, 200) __CCM__;
static THD_FUNCTION(TlmSenderThread, arg) {
  chRegSetThreadName("TLM_Scheduler");
  (void)arg;
  systime_t t; /* milliseconds to sleep to next deadline */

  /* main loop */
  while (!chThdShouldTerminateX()){
    if (true == pause_flag)
      chThdSleepMilliseconds(100);
    else{
      t = get_sleep_time(Registry, sizeof(Registry)/sizeof(Registry[0]));
      chThdSleepMilliseconds(t);
      refresh_deadlines(Registry, sizeof(Registry)/sizeof(Registry[0]), t);
    }
  }

  chThdExit(MSG_OK);
}

/** 
 *
 */
static void load_parameters(void) {
  param_registry.valueSearch("T_attitude", &(Registry[0].sleepperiod));
  param_registry.valueSearch("T_debug", &(Registry[1].sleepperiod));
  param_registry.valueSearch("T_debug_vect", &(Registry[2].sleepperiod));
  param_registry.valueSearch("T_global_pos", &(Registry[3].sleepperiod));
  param_registry.valueSearch("T_gps_raw_int", &(Registry[4].sleepperiod));
  param_registry.valueSearch("T_highres_imu", &(Registry[5].sleepperiod));
  param_registry.valueSearch("T_mission_curr", &(Registry[6].sleepperiod));
  param_registry.valueSearch("T_nav_output", &(Registry[7].sleepperiod));
  param_registry.valueSearch("T_position_ned", &(Registry[8].sleepperiod));
  param_registry.valueSearch("T_raw_imu", &(Registry[9].sleepperiod));
  param_registry.valueSearch("T_raw_press", &(Registry[10].sleepperiod));
  param_registry.valueSearch("T_rc", &(Registry[11].sleepperiod));
  param_registry.valueSearch("T_rc_scaled", &(Registry[12].sleepperiod));
  param_registry.valueSearch("T_scal_press", &(Registry[13].sleepperiod));
  param_registry.valueSearch("T_sys_status", &(Registry[14].sleepperiod));
  param_registry.valueSearch("T_vfr_hud", &(Registry[15].sleepperiod));
}

/*
 *******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************
 */
/**
 *
 */
TlmSender::TlmSender(void){
  return;
}

/**
 *
 */
void TlmSender::start(void){

  load_parameters();

  this->worker = chThdCreateStatic(TlmSenderThreadWA,
                 sizeof(TlmSenderThreadWA),
                 TELEMETRYDISPPRIO,
                 TlmSenderThread,
                 NULL);
  osalDbgCheck(NULL != this->worker);

  pause_flag = false;
}

/**
 *
 */
void TlmSender::stop(void){
  pause_flag = true;
  chThdTerminate(worker);
  chThdWait(worker);
}

/**
 *
 */
void TlmSender::pause(void){
  pause_flag = true;
}

/**
 *
 */
void TlmSender::resume(void){
  pause_flag = false;
}
