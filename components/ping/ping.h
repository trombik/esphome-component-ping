#pragma once

#include "AsyncPing.h"

#include "esphome/components/sensor/sensor.h"

#define EACH_RESULT true
#define END false
#define TAG "ping"

namespace esphome {
namespace ping {

class PingSensor : public sensor::Sensor, public PollingComponent {
    public:
        Sensor *packet_loss_sensor = new Sensor();
        Sensor *latency_sensor = new Sensor();

        /* the sensor must be started after connecting WiFi */
        float get_setup_priority() const override { return esphome::setup_priority::AFTER_WIFI; }

        void setup() override {

            ping.on(EACH_RESULT,  [this](const AsyncPingResponse& response) {
                    if (response.answer) {
                        ESP_LOGI(TAG, "%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms",
                                response.size,
                                target.c_str(),
                                response.icmp_seq,
                                response.ttl,
                                response.time);

                        this->incr_total_success_time(response.time);
                    } else {
                        ESP_LOGI(TAG, "no reply from %s", target.c_str());
                    }
                    return false;
                    });

            /* at the end, set the result */
            ping.on(END,
                    [this](const AsyncPingResponse& response) {
                        float loss = 0;
                        int total_failed_count = response.total_sent - response.total_recv;
                        if (response.total_sent != 0) {
                            loss = (float)total_failed_count / response.total_sent;
                        }

                        int mean = 0;
                        if (response.total_recv != 0) {
                            mean = total_success_time / response.total_recv;
                        }

                        this->set_latest_loss(loss * 100);
                        this->set_latest_latency(mean);

                        ESP_LOGI(TAG, "packet loss: %0.1f %% latency: %d ms", loss * 100, mean);
                        this->reset();
                        return true;
                    }
            );
            ping.begin(target.c_str(), n_packet, timeout);
        }

        void update() override {
            float loss;
            int latency_ms;

            loss = this->get_latest_loss();
            latency_ms = this->get_latest_latency();

            if (loss >= 0 && this->packet_loss_sensor_ != nullptr) {
                packet_loss_sensor_->publish_state(loss);
            }
            if (latency_ms >= 0 && this->latency_sensor_ != nullptr) {
                latency_sensor_->publish_state((float)latency_ms / 1000);
            }
            ping.begin(target.c_str(), n_packet, timeout);
        }

        int get_latest_latency() {
            return latest_latency;
        }

        float get_latest_loss() {
            return latest_loss;
        }

        void set_timeout(uint32_t timeout_ms) {
            timeout = timeout_ms;
        }

        void set_target(const std::string address) {
            target = address;
        }

        void set_n_packet(uint32_t n) {
            n_packet = n;
        }

        void set_packet_loss_sensor(sensor::Sensor *packet_loss_sensor) {
            packet_loss_sensor_ = packet_loss_sensor;
        }

        void set_latency_sensor(sensor::Sensor *latency_sensor) {
            latency_sensor_ = latency_sensor;
        }

    private:
        void set_latest_loss(float f) {
            latest_loss = f;
        }

        void set_latest_latency(int i) {
            latest_latency = i;
        }

        void reset() {
            total_success_time = 0;
        }

        void incr_total_success_time(int time) {
            total_success_time += time;
        }
    protected:
        float latest_loss = -1;
        int latest_latency = -1;
        int total_success_time;
        std::string target = "8.8.8.8";
        int timeout = 1000;
        int n_packet = 13;

        AsyncPing ping;
        sensor::Sensor *packet_loss_sensor_{nullptr};
        sensor::Sensor *latency_sensor_{nullptr};
};

}  // namespace ping
}  // namespace esphome
