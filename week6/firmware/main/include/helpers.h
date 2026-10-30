#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

//CONSTANT DEFINITIONS
#define VRX_PIN 36
#define VRY_PIN 39
#define SW_PIN 32
#define LED_PIN 25

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

#define LCD_ADDR             0x27
#define FLAG_BACKLIGHT_ON    0x08
#define FLAG_ENABLE          0x04
#define FLAG_RS_DATA         0x01
#define FLAG_RS_COMMAND      0x00

//SETUP FUNCTIONS
void timer_setup(uint32_t res_hz, uint64_t alarm_hz);
void setup_lcd();
void setup_joystick();

//LCD FUNCTIONS
void lcd_print(const char *str);
void lcd_print_binary(const char* prefix, uint8_t val);
void lcd_set_cursor(bool row);

//GLOBAL ISR FLAG
extern volatile bool timer_triggered;

//ADC Handle
extern adc_oneshot_unit_handle_t adc1_handle;

#endif