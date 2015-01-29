#include "main.h"

#include <futaba/receiver_synth.hpp>

using namespace control;

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

/*
 ******************************************************************************
 ******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************
 ******************************************************************************
 */

/*
 ******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************
 */
/**
 *
 */
ReceiverSynth::ReceiverSynth(systime_t timeout) : Receiver(timeout) {
  return;
}

/**
 *
 */
void ReceiverSynth::start(void) {
  ready = true;
}

/**
 *
 */
void ReceiverSynth::stop(void) {
  ready = false;
}

/**
 *
 */
msg_t ReceiverSynth::update(uint16_t *pwm) const {

  msg_t ret = MSG_OK;

  chDbgCheck(ready);

  for (size_t i=0; i<FUTABA_RECEIVER_PWM_CHANNELS; i++)
    pwm[i] = 1500;

  ret = MSG_TIMEOUT;
  return ret;
}




