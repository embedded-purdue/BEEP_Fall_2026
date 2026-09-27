#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>

//CONSTANT DEFINITIONS
#define GREEN_LED_PIN 27
#define RED_LED_PIN 14
#define BLUE_LED_PIN 12

#define ARM_PIN 18
#define DISARM_PIN 19
#define CODE0_PIN 21
#define CODE1_PIN 22
#define SETCODE_PIN 23

#define DEBOUNCE_TIME 100000
#define DUTY_CYCLE_MAX 1000

//SETUP FUNCTIONS
void timer_setup(uint32_t res_hz, uint64_t alarm_hz);
void pwm_setup();
void rgb_strip_setup();

//GLOBAL ISR FLAG
extern volatile bool timer_triggered;

//BONUS: RMT Configs for RGB LED Strip
#define RGB_STRIP_PIN 17
#define STRIP_TICK_HZ 20000000

#define T0H_TICKS 8
#define T0L_TICKS 17
#define T1H_TICKS 16
#define T1L_TICKS 9

#endif