#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"

// Suggested pins; change these to match your wiring.
static const gpio_num_t BUTTON_PIN = GPIO_NUM_13;
static const gpio_num_t SIGNAL_LED_PIN = GPIO_NUM_19;

void app_main(void)
{
    // TODO: Configure the button input and LED output.
    // TODO: Add a game state machine: READY -> WAITING -> GO -> RESULT.
    // TODO: Use esp_timer_get_time() to measure reaction time in microseconds.
    // TODO: Add debounce, false-start handling, random waiting, and replay.
    ESP_LOGI("REACTION", "Capstone starter: build your reaction-time game.");
    (void)BUTTON_PIN;
    (void)SIGNAL_LED_PIN;
}
