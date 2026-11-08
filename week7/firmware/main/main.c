#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h" 
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "helpers.h"

const char* TAG = "MAIN";

QueueHandle_t distance_queue;

// ------------------------ Helper Functions ------------------------

// Waits for ECHO_PIN to reach `level`, giving up after ECHO_TIMEOUT_US.
// This matters because a disconnected/miswired sensor would otherwise leave
// ECHO stuck low forever, which would spin this loop indefinitely and never
// let the task yield the CPU (a common way to trip the watchdog timer).
// Returns true and writes the transition timestamp into *timestamp on success.
static bool wait_for_echo_level(int level, int64_t *timestamp) {
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(ECHO_PIN) != level) {
        if ((esp_timer_get_time() - start) > ECHO_TIMEOUT_US) {
            return false;
        }
    }
    *timestamp = esp_timer_get_time();
    return true;
}

// ------------------------ Tasks ------------------------

// TODO: Implement ultrasonic_task.
// This is a FreeRTOS task (note the infinite `while(1)` - it never returns)
// that should, every loop:
//   1. Pulse TRIG_PIN: low, delay 2us (ets_delay_us), high, delay 10us, low.
//      This is the trigger pulse the HC-SR04 datasheet requires.
//   2. Call wait_for_echo_level(1, &echo_start) to catch the rising edge of
//      ECHO. If it returns false (timeout), log a warning, vTaskDelay 500ms,
//      and `continue` to skip the rest of this iteration.
//   3. Call wait_for_echo_level(0, &echo_end) to catch the falling edge.
//      Same timeout handling as step 2.
//   4. Calculate distance_cm from (echo_end - echo_start), the speed of
//      sound, and the fact that the pulse travels there AND back:
//        float distance_cm = (echo_end - echo_start) * 0.0343 / 2.0;
//   5. Push distance_cm onto distance_queue with xQueueSend().
//   6. vTaskDelay 500ms before the next reading.
void ultrasonic_task(void *pvParameters) {
    while(1) {
        // TODO: fill in
    }
}

// TODO: Implement lcd_display_task.
// This is a FreeRTOS task that should sit blocked on distance_queue and,
// whenever a new reading arrives, format it into a string (e.g. with
// snprintf, "Dist: %.2f cm") and print it to the LCD's top row.
//   - Use xQueueReceive(distance_queue, &received_distance, portMAX_DELAY)
//     to block indefinitely until data shows up (this costs zero CPU cycles
//     while waiting - see 7.1.5 in the README).
//   - Remember to check the return value is pdPASS before using the data.
void lcd_display_task(void *pvParameters) {
    float received_distance;
    char buffer[16];

    while(1) {
        // TODO: fill in
    }
}

// TODO: Implement app_main.
//   1. Call setup_lcd() and setup_ultrasonic().
//   2. Create distance_queue with xQueueCreate() - it needs to hold floats,
//      a depth of 5 is plenty.
//   3. Create both tasks with xTaskCreate(). Give lcd_display_task a HIGHER
//      priority number than ultrasonic_task (see 7.1.4 in the README for
//      why) - e.g. priority 6 for the display task, 5 for the sensor task.
//      A stack size of 2048 words is enough for both.
void app_main(void) {
    // TODO: fill in
}
