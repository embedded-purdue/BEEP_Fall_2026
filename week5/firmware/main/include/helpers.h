#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

//CONSTANT DEFINITIONS
#define GREEN_LED_PIN 27
#define RED_LED_PIN 14
#define BLUE_LED_PIN 12

#define ARM_PIN 18
#define DISARM_PIN 19
#define CODE0_PIN 5
#define CODE1_PIN 17
#define SETCODE_PIN 23

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

#define DEBOUNCE_TIME 100000
#define DUTY_CYCLE_MAX 1000

#define LCD_ADDR             0x27
#define FLAG_BACKLIGHT_ON    0x08
#define FLAG_ENABLE          0x04
#define FLAG_RS_DATA         0x01
#define FLAG_RS_COMMAND      0x00

//SETUP FUNCTIONS
void timer_setup(uint32_t res_hz, uint64_t alarm_hz);
void pwm_setup();

//LCD FUNCTIONS
void setup_lcd();
void lcd_print(const char *str);
void lcd_print_binary(const char* prefix, uint8_t val);
void lcd_set_cursor(bool row);

//GLOBAL ISR FLAG
extern volatile bool timer_triggered;

#endif