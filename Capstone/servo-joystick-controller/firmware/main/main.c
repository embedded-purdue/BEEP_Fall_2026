#include <stdio.h>
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"

#define JOYSTICK_X_CHANNEL ADC_CHANNEL_0  // GPIO36
#define JOYSTICK_Y_CHANNEL ADC_CHANNEL_3  // GPIO39
#define JOYSTICK_SW_PIN    32
#define SERVO_GPIO         18

void app_main(void)
{
    // TODO: Configure ADC1 for the joystick axes and a GPIO input for the switch.
    // TODO: Configure LEDC PWM for a 50 Hz servo signal.
    // TODO: Calibrate the center, add a dead zone/smoothing, and map to safe angles.
    // TODO: Decide what the joystick switch or an added control should do.
    ESP_LOGI("SERVO", "Capstone starter: build your joystick controller.");
    (void)JOYSTICK_X_CHANNEL;
    (void)JOYSTICK_Y_CHANNEL;
    (void)JOYSTICK_SW_PIN;
    (void)SERVO_GPIO;
}
