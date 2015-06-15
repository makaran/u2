#ifndef GNSS_RECEIVER_HPP_
#define GNSS_RECEIVER_HPP_

#include "acs_input.hpp"
#include "gnss_data.hpp"
#include "ubx_proto.hpp"

#define EVMSK_GNSS_FRESH_VALID    (1UL << 0)

extern chibios_rt::EvtSource event_gps;

#define GNSS_MAX_SUBSCRIBERS      4

namespace gnss {

/**
 *
 */
class GNSSReceiver {
public:
  GNSSReceiver(void);
  void start(void);
  void stop(void);
  void getCache(gnss::gnss_data_t &result);
  void subscribe(gnss::gnss_data_t* result);
  void unsubscribe(gnss::gnss_data_t* result);
  void setSniffer(SerialDriver *sdp);
  void deleteSniffer(void);
  static void GNSS_PPS_ISR_I(void);
private:
  static THD_FUNCTION(nmeaRxThread, arg);
  static THD_FUNCTION(ubxRxThread, arg);
  void update_settings(void);
  void pvtdispatch(const ubx_nav_pvt &pvt);
  gnss_data_t* spamlist[GNSS_MAX_SUBSCRIBERS] = {};
  bool ready = false;
  thread_t *worker = nullptr;
  SerialDriver *sniff_sdp = nullptr;
  const uint32_t *dyn_model = nullptr;
  const uint32_t *fix_period = nullptr;
  uint32_t dyn_model_cache = 8;
  uint32_t fix_period_cache = 200;
  gnss_data_t cache;
};

} // namespace

#endif /* GNSS_RECEIVER_HPP_ */
