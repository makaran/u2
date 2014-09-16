#ifndef MAVWORKER_HPP_
#define MAVWORKER_HPP_

#include "main.h"
#include "mavchannel.hpp"
#include "subscribe_link.hpp"
#include "mavmail.hpp"

#if !MAVLINK_UNHANDLED_MSG_DEBUG
#define WORKER_RX_THREAD_WA_SIZE  512
#else
#define WORKER_RX_THREAD_WA_SIZE  2048
#endif

#define WORKER_TX_THREAD_WA_SIZE  1024

#define MAVCHANNEL_BUF_SIZE     MAVLINK_SENDBUF_SIZE
#define MAVCHANNEL_BUF_CNT      3

#if (MAVCHANNEL_BUF_CNT < 2)
#error "Buffer count can not be less than 2"
#endif

class MavWorker {
public:
  MavWorker(void);
  void start(mavChannel *channel);
  void stop(void);
  msg_t post(mavMail &mail);
private:
  mavChannel *channel = NULL;
  thread_t *rxworker = NULL;
  thread_t *txworker = NULL;
  bool ready = false;
};

extern MavWorker mav_worker;

#endif /* MAVWORKER_HPP_ */
