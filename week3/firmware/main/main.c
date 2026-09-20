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
// TODO: Fill in the blanks. Same idea as last week, but this time we ALSO
// need to enable interrupts on these pins. Buttons are active-low, so you
// want the interrupt to fire on the falling edge (press), not the rising
// edge (release). Look at gpio_int_type_t in the ESP-IDF docs for the
// right enum value.
gpio_config_t input_pin = {
    .pin_bit_mask = (1ULL << ARM_PIN) |
                    (1ULL << DISARM_PIN) |
                    (1ULL << CODE0_PIN) |
                    (1ULL << CODE1_PIN) |
                    (1ULL << SETCODE_PIN),
    .mode = /* TODO */,
    .pull_up_en = /* TODO */,
    .pull_down_en = /* TODO */,
    .intr_type = /* TODO: which edge? */
};

// Output pin config
// TODO: Fill in the blanks (unchanged concept from last week - LEDs are
// simple outputs, no pull resistors, no interrupts needed).
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

// Flags for print statements - since we can't printf/log from an ISR, each
// handler sets one of these and the main loop does the actual logging.
volatile bool newly_armed = false;
volatile bool newly_disarmed = false;
volatile bool new_code_set = false;
volatile bool newly_triggered = false;

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

bool debounce_check(intmax_t* last_press) {
    int64_t now = esp_timer_get_time();
    if ((now - *last_press) > DEBOUNCE) {
        *last_press = now;
        return true;
    }
    return false;
}

// ------------------------ Interrupt Handlers -----------------------
// Remember: ISRs must be SHORT. No printf/ESP_LOGI here - set a flag instead
// and let the main loop do the logging. Also remember these fire on a FALLING
// edge (press only), so you don't need to check gpio_get_level() inside them
// the way you did with polling last week - if the ISR ran, the button was
// just pressed.

// ARM
// TODO: If debounce passes AND the alarm isn't already armed/setting a code,
// arm the alarm, set newly_armed = true, and reset guess_code.
static void handle_arm_press(void *arg) {
    // TODO: fill in
}

// DISARM
// TODO: If debounce passes and the alarm is armed, compare guess_code to
// code. Match -> disarm, clear alarm_triggered, reset guess_code, set
// newly_disarmed. No match -> set alarm_triggered and newly_triggered.
static void handle_disarm_press(void *arg) {
    // TODO: fill in
}

// SETCODE
// TODO: If debounce passes: first press (not armed, not already setting)
// enters setting_code mode and resets temp_code. Second press (setting_code
// already true) saves temp_code into code, exits setting_code mode, and sets
// new_code_set.
static void handle_setcode_press(void *arg) {
    // TODO: fill in
}

// CODE 0
// TODO: If debounce passes: while setting a code, shift a 0 bit into
// temp_code. While armed (guessing), shift a 0 bit into guess_code.
static void handle_code0_press(void *arg) {
    // TODO: fill in
}

// CODE 1
// TODO: Same as CODE 0's handler, but shifts in a 1 bit instead of a 0.
static void handle_code1_press(void *arg) {
    // TODO: fill in
}

// This function is unchanged from Week 2 - it's provided for you.
void update_leds(void) {
    // Armed/Disarmed LEDs
    if (!alarm_triggered) {
        gpio_set_level(RED_LED_PIN, alarm_armed ? 1 : 0);
        gpio_set_level(GREEN_LED_PIN, alarm_armed ? 0 : 1);
    }

    // Triggered alarm flashing
    if (alarm_triggered && (esp_timer_get_time() - previous_alarm_beep > ALARM_PERIOD)) {
        alarm_flash = !alarm_flash;
        gpio_set_level(RED_LED_PIN, alarm_flash);
        previous_alarm_beep = esp_timer_get_time();
    }

    // Setting code LED
    gpio_set_level(BLUE_LED_PIN, setting_code ? 1 : 0);
}

// TODO: Implement setup_gpio_irq.
// You need to:
//   1. Install the GPIO ISR service (gpio_install_isr_service) so the
//      ESP-IDF can dispatch to a different handler per pin.
//   2. Register each of the 5 handlers above to their matching pin with
//      gpio_isr_handler_add(pin, handler_function, arg).
void setup_gpio_irq() {
    // TODO: fill in
}

// ------------------------ Main Application ------------------------

void app_main(void) {
    // Setup
    gpio_config(&input_pin);
    gpio_config(&output_pin);

    setup_gpio_irq();

    gpio_set_level(RED_LED_PIN, 0);
    gpio_set_level(BLUE_LED_PIN, 0);
    gpio_set_level(GREEN_LED_PIN, 0);

    vTaskDelay(pdMS_TO_TICKS(100));

    // Main loop
    while (1) {
        update_leds();

        if (newly_armed) {
            newly_armed = false;
            ESP_LOGI(TAG, "SYSTEM ARMED!\n");
        }
        if (newly_disarmed) {
            newly_disarmed = false;
            ESP_LOGI(TAG, "SYSTEM DISARMED!\n");
        }
        if (newly_triggered) {
            newly_triggered = false;
            ESP_LOGI(TAG, "ALARM TRIGGERED!\n");
        }
        if (new_code_set) {
            new_code_set = false;
            print_code(code, "New Code Set:");
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
