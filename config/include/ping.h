#include "esphome.h"
#include "AsyncPing.h"

#define TARGET "8.8.8.8"
#define TIMEOUT (1000)
#define N_PACKET (13)
#define TIME_POLLING_SEC (120)

#define EACH_RESULT true
#define END false
#define TAG "ping"

class PingSensor : public PollingComponent, public Sensor {
    public:
        float latest_loss = -1;
        int latest_latency = -1;
        int total_success_time;
        AsyncPing ping;

        Sensor *packet_loss_sensor = new Sensor();
        Sensor *latency_sensor = new Sensor();

        PingSensor() : PollingComponent(TIME_POLLING_SEC * 1000) {}

        /* the sensor must be started after connecting WiFi */
        float get_setup_priority() const override { return esphome::setup_priority::AFTER_WIFI; }

        void setup() override {

            ping.on(EACH_RESULT,  [this](const AsyncPingResponse& response) {
                    if (response.answer) {
                        ESP_LOGI(TAG, "%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms",
                                response.size,
                                TARGET,
                                response.icmp_seq,
                                response.ttl,
                                response.time);

                        this->incr_total_success_time(response.time);
                    } else {
                        ESP_LOGI(TAG, "no reply from %s", TARGET);
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
            ping.begin(TARGET, N_PACKET, TIMEOUT);
        }

        void update() override {
            float loss;
            int latency_ms;

            loss = this->get_latest_loss();
            latency_ms = this->get_latest_latency();

            if (loss >= 0) {
                packet_loss_sensor->publish_state(loss);
            }
            if (latency_ms >= 0) {
                latency_sensor->publish_state(latency_ms);
            }
            ping.begin(TARGET, N_PACKET, TIMEOUT);
        }

        int get_latest_latency() {
            return latest_latency;
        }

        float get_latest_loss() {
            return latest_loss;
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
};
