#include <stdio.h>
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"

// Suggested ADC pins; use ADC1 pins so Wi-Fi does not interfere with readings.
#define PHOTO_ADC_CHANNEL ADC_CHANNEL_6  // GPIO34
#define POT_ADC_CHANNEL   ADC_CHANNEL_4  // GPIO32

void app_main(void)
{
    // TODO: Configure one ADC1 oneshot unit and these two channels.
    // TODO: Read the photoresistor divider and potentiometer.
    // TODO: Smooth or threshold the sensor readings.
    // TODO: Configure three PWM channels for the RGB LED.
    // TODO: Add your chosen brightness/color mapping and calibration behavior.
    ESP_LOGI("NIGHT_LIGHT", "Capstone starter: build your adaptive light.");
    (void)PHOTO_ADC_CHANNEL;
    (void)POT_ADC_CHANNEL;
}
