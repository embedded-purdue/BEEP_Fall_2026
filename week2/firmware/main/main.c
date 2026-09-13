#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"

const char* TAG = "MAIN";

#define GREEN_LED_PIN 27
#define RED_LED_PIN 14
#define BLUE_LED_PIN 12

#define ARM_PIN 18
#define DISARM_PIN 19
#define CODE0_PIN 21
#define CODE1_PIN 22
#define SETCODE_PIN 23

#define DEBOUNCE 250 * 1000
#define ALARM_PERIOD 200 * 1000

// Input pin config
// TODO: Fill in the blanks below. This config needs to describe:
//   - the 5 button pins (ARM, DISARM, CODE0, CODE1, SETCODE) via pin_bit_mask
//   - INPUT mode
//   - internal pull-ups ENABLED, pull-downs DISABLED (buttons are active-low)
//   - interrupts DISABLED (we are polling this week, not using interrupts yet)
gpio_config_t input_pin = {
    .pin_bit_mask = (1ULL << ARM_PIN) |
                    (1ULL << DISARM_PIN) |
                    (1ULL << CODE0_PIN) |
                    (1ULL << CODE1_PIN) |
                    (1ULL << SETCODE_PIN),
    .mode = /* TODO */,
    .pull_up_en = /* TODO */,
    .pull_down_en = /* TODO */,
    .intr_type = /* TODO */
};

// Output pin config
// TODO: Fill in the blanks below. This config needs to describe:
//   - the 3 LED pins (RED, GREEN, BLUE) via pin_bit_mask
//   - OUTPUT mode
//   - no pull-up/pull-down needed on outputs
//   - interrupts DISABLED
gpio_config_t output_pin = {
    .pin_bit_mask = (1ULL << RED_LED_PIN) |
                    (1ULL << GREEN_LED_PIN) |
                    (1ULL << BLUE_LED_PIN),
    .mode = /* TODO */,
    .pull_up_en = /* TODO */,
    .pull_down_en = /* TODO */,
    .intr_type = /* TODO */
};

// Alarm variables
bool alarm_triggered = false;
bool alarm_flash = false;
bool alarm_armed = false;
bool setting_code = false;
uint8_t code = 0b00000000;
uint8_t temp_code = 0b00000000;
uint8_t guess_code = 0b00000000;

// Debounce variables
intmax_t previous_alarm_beep = 0;
intmax_t previous_arm_press = 0;
intmax_t previous_disarm_press = 0;
intmax_t previous_setcode_press = 0;
intmax_t previous_code1_press = 0;
intmax_t previous_code0_press = 0;

// ------------------------ Helper Functions ------------------------

void print_code(uint8_t val, const char* msg) {
    ESP_LOGI(TAG, "%s: %c%c%c%c%c%c%c%c", msg,
        val & 0x80 ? '1' : '0',
        val & 0x40 ? '1' : '0',
        val & 0x20 ? '1' : '0',
        val & 0x10 ? '1' : '0',
        val & 0x08 ? '1' : '0',
        val & 0x04 ? '1' : '0',
        val & 0x02 ? '1' : '0',
        val & 0x01 ? '1' : '0');
}

// Returns true if enough time (DEBOUNCE microseconds) has passed since the
// last accepted press. If true, it also updates *last_press to now.
bool debounce_check(intmax_t* last_press) {
    int64_t now = esp_timer_get_time();
    if ((now - *last_press) > DEBOUNCE) {
        *last_press = now;
        return true;
    }
    return false;
}

// TODO: Implement handle_arm_disarm.
// This function should, using debounce_check() and gpio_get_level():
//   - ARM: if the alarm is not armed, not currently setting a code, and the
//     ARM_PIN is pressed (active low) and passes debounce -> set alarm_armed
//     true, log it, and reset guess_code to 0.
//   - DISARM: if the alarm IS armed and DISARM_PIN is pressed and passes
//     debounce -> compare guess_code to code. If they match, disarm and clear
//     alarm_triggered + guess_code and log it. If they don't match, set
//     alarm_triggered true and log it.
void handle_arm_disarm(void) {
    // TODO: fill in
}

// TODO: Implement handle_set_code.
// This function should handle the SETCODE button as a two-press sequence:
//   - First SETCODE press (only if not armed and not already setting a code):
//     enter "setting_code" mode and reset temp_code to 0.
//   - While setting_code is true: CODE0 press shifts a 0 bit into temp_code,
//     CODE1 press shifts a 1 bit into temp_code (use print_code to log the
//     value after each shift).
//   - Second SETCODE press (while setting_code is true): save temp_code into
//     code, leave setting_code mode, and log the new code.
// Use debounce_check() before acting on any button read.
void handle_set_code(void) {
    // TODO: fill in
}

// TODO: Implement handle_guess_code.
// Only runs when the alarm is armed. CODE0 shifts a 0 bit into guess_code,
// CODE1 shifts a 1 bit into guess_code (again, log with print_code). Don't
// forget to debounce_check() each button.
void handle_guess_code(void) {
    // TODO: fill in
}

// TODO: Implement update_leds.
// This function should read the current program state and update the LEDs:
//   - If the alarm is not triggered: RED on / GREEN off when armed, RED off /
//     GREEN on when disarmed.
//   - If the alarm IS triggered: flash the RED LED on/off every ALARM_PERIOD
//     microseconds (use esp_timer_get_time() and previous_alarm_beep to time
//     this, and the alarm_flash bool to track on/off state).
//   - BLUE LED should be on whenever setting_code is true, off otherwise.
void update_leds(void) {
    // TODO: fill in
}

// ------------------------ Main Application ------------------------

void app_main(void) {
    // TODO: Setup - call gpio_config() on both input_pin and output_pin so
    // the ESP-IDF actually applies the configs you filled in above.


    // TODO: Set all three LEDs (RED, BLUE, GREEN) to a known starting state
    // (off) before entering the main loop.


    vTaskDelay(pdMS_TO_TICKS(100));

    // Main loop
    while (1) {
        handle_arm_disarm();
        handle_set_code();
        handle_guess_code();
        update_leds();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
