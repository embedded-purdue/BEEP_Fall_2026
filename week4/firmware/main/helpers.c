#include <stdio.h>
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/rmt_tx.h"
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include "helpers.h"

//Timer Handle
gptimer_handle_t timer_handle;

//Timer ISR Flag
volatile bool timer_triggered = false;

//BONUS: RMT Config
rmt_channel_handle_t rmt_handle;
rmt_encoder_handle_t encoder_handle;
rmt_transmit_config_t rmt_tx_config = {
    .loop_count = 0,
    .flags.eot_level = 0,
};

// ----------------- Timer ISR -----------------

static bool timer_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    timer_triggered = true;
    return false;
}

// ----------------- Setup Functions -----------------

// TODO: Implement pwm_setup.
// This needs to configure the LEDC peripheral for 3 PWM channels (one per
// RGB color pin), all sharing one base timer:
//   1. An ledc_timer_config_t: high-speed mode, some duty resolution (e.g.
//      LEDC_TIMER_10_BIT gives duty values 0-1024), a timer number, a
//      frequency (10KHz is a good default), and LEDC_AUTO_CLK. Apply it with
//      ledc_timer_config().
//   2. An ledc_channel_config_t PER LED (red/green/blue), each pointing at
//      its own gpio_num and a unique LEDC_CHANNEL_n, referencing the timer
//      you just configured, starting at duty = 0. Apply each with
//      ledc_channel_config().
void pwm_setup() {
    // TODO: fill in
}

// TODO: Implement timer_setup(res_hz, alarm_hz).
// This should configure a General Purpose Timer (GPTimer) that fires
// `timer_handler` periodically:
//   1. A gptimer_config_t: clock source SOC_MOD_CLK_APB, count direction UP,
//      resolution_hz = the res_hz argument.
//   2. A gptimer_alarm_config_t: alarm_count = the alarm_hz argument,
//      reload_count = 0, and auto_reload_on_alarm = true so it repeats.
//   3. A gptimer_event_callbacks_t pointing on_alarm at timer_handler.
//   4. Create the timer (gptimer_new_timer), apply the alarm config
//      (gptimer_set_alarm_action), register the callbacks
//      (gptimer_register_event_callbacks), then enable and start it
//      (gptimer_enable, gptimer_start).
void timer_setup(uint32_t res_hz, uint64_t alarm_hz) {
    // TODO: fill in
}
