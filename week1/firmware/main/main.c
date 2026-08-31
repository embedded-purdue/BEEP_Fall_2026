#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"

#define LED_PIN 27
#define BUTTON_PIN 26

void app_main(void) {
  int button_state;

  // Configure LED pin
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  // Configure Button pin
  gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
  gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLUP_ONLY);

  while (1) {
    button_state = gpio_get_level(BUTTON_PIN);

    // Button is active low (pulled up)
    if (button_state == 0) {
      gpio_set_level(LED_PIN, 1);
    } else {
      gpio_set_level(LED_PIN, 0);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}