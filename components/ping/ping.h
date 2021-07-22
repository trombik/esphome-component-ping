#pragma once

#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ping {
class PingSensor : public sensor::Sensor, public PollingComponent {
 public:
  Sensor *packet_loss_sensor = new Sensor();
  Sensor *latency_sensor = new Sensor();

  /* the sensor must be started after connecting WiFi */
  float get_setup_priority() const override { return esphome::setup_priority::AFTER_WIFI; }

  void set_n_packet(uint32_t n) { n_packet = n; }
  void set_target(const std::string address) { target = address; }
  void set_timeout(uint32_t timeout_ms) { timeout = timeout_ms; }

  int get_latest_latency() { return latest_latency; }
  float get_latest_loss() { return latest_loss; }

  void set_packet_loss_sensor(sensor::Sensor *packet_loss_sensor) { packet_loss_sensor_ = packet_loss_sensor; }
  void set_latency_sensor(sensor::Sensor *latency_sensor) { latency_sensor_ = latency_sensor; }

 private:
 protected:
  int latest_latency = -1;
  int timeout = 1000;
  int n_packet = 13;
  float latest_loss = -1;
  std::string target = "8.8.8.8";
  sensor::Sensor *packet_loss_sensor_{nullptr};
  sensor::Sensor *latency_sensor_{nullptr};

  void set_latest_loss(float f) { latest_loss = f; }
  void set_latest_latency(int i) { latest_latency = i; }
};

}  // namespace ping
}  // namespace esphome
