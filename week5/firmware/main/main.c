#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "helpers.h"

const char* TAG = "MAIN";

// Input pin config
gpio_config_t input_pin = {
    .pin_bit_mask = (1ULL << ARM_PIN) |
                    (1ULL << DISARM_PIN) |
                    (1ULL << CODE0_PIN) |
                    (1ULL << CODE1_PIN) |
                    (1ULL << SETCODE_PIN),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_NEGEDGE
};

// Alarm State
bool alarm_triggered = false;
bool alarm_armed = false;
bool setting_code = false;
uint8_t code = 0b00000000;
uint8_t temp_code = 0b00000000;
uint8_t guess_code = 0b00000000;

//RGB LED
int brightness = 0;
int step = 50;

// Button Debounce Timestamps
intmax_t previous_arm_press = 0;
intmax_t previous_disarm_press = 0;
intmax_t previous_setcode_press = 0;
intmax_t previous_code1_press = 0;
intmax_t previous_code0_press = 0;

//ISR Flags for Print Statements
volatile bool newly_armed = false;
volatile bool newly_disarmed = false;
volatile bool first_code_press = false;
volatile bool second_code_press = false;
volatile bool newly_triggered = false;
volatile bool code_button_pressed = false;


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
    if ((now - *last_press) > DEBOUNCE_TIME) {
        *last_press = now;
        return true;
    }
    return false;
}

void update_leds(void) {
    //Base Set Conditions
    ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2, (setting_code && !alarm_armed && !alarm_triggered) ? DUTY_CYCLE_MAX : 0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1, (!setting_code && !alarm_armed && !alarm_triggered) ? DUTY_CYCLE_MAX : 0);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0, (alarm_armed) ? DUTY_CYCLE_MAX : 0);

    //Red Alarm Flashing
    if (alarm_triggered && timer_triggered) {
        timer_triggered = false;
        brightness += step;
        if ((brightness == 0) || (brightness == DUTY_CYCLE_MAX)) {
            step *= -1;
        }
        ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0,brightness);
    }

    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0); 
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1); 
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
}

// ------------------------ Interrupt Handlers -----------------------

// ARM
static void handle_arm_press(void *arg) {
    if (!debounce_check(&previous_arm_press)) {return;}

    if (!alarm_armed && !setting_code) {
        alarm_armed = true;
        newly_armed = true;
        guess_code = 0b00000000;
    }
}

// DISARM
static void handle_disarm_press(void *arg) {
    if (!debounce_check(&previous_disarm_press)) {return;}

    if (alarm_armed) {
        if (guess_code == code) {
            alarm_armed = false;
            alarm_triggered = false;
            newly_disarmed = true;
            guess_code = 0b00000000;
        } else {
            newly_triggered = true;
            alarm_triggered = true;
        }
    }
}

// SETCODE
static void handle_setcode_press(void *arg) {
    if (!debounce_check(&previous_setcode_press)) {return;}

    // first press
    if (!alarm_armed && !setting_code) {
        setting_code = true;
        first_code_press = true;
        temp_code = 0;
    }
    // second press
    else if (setting_code) {
        code = temp_code;
        setting_code = false;
        second_code_press = true;
    }
}

// CODE 0
static void handle_code0_press(void *arg) {
    if (!debounce_check(&previous_code0_press)) {return;}

    code_button_pressed = true;
    // Code set press
    if (setting_code) {
        temp_code <<= 1;
    }
    // Code guess press
    else if (alarm_armed) {
        guess_code <<= 1;
    }
}

// CODE 1
static void handle_code1_press(void *arg) {
    if (!debounce_check(&previous_code1_press)) {return;}

    code_button_pressed = true;
    // Code set press
    if (setting_code) {
        temp_code = (temp_code << 1) | 1;
    }
    // Code guess press
    else if (alarm_armed) {
        guess_code = (guess_code << 1) | 1;
    }
}

// ------------------------ Setup -----------------------

void setup_gpio_irq() {
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);

    gpio_isr_handler_add(ARM_PIN,handle_arm_press,NULL);
    gpio_isr_handler_add(DISARM_PIN,handle_disarm_press,NULL);
    gpio_isr_handler_add(SETCODE_PIN,handle_setcode_press,NULL);
    gpio_isr_handler_add(CODE0_PIN,handle_code0_press,NULL);
    gpio_isr_handler_add(CODE1_PIN,handle_code1_press,NULL);
}

// ------------------------ Main Application ------------------------

void app_main(void) {
    //Setup
    gpio_config(&input_pin);
    setup_gpio_irq();
    pwm_setup();
    timer_setup(1000000, 10000); //1us ticks, 100Hz alarm
    setup_lcd();

    lcd_set_cursor(0);
    lcd_print("Welcome to BEEP");
    lcd_set_cursor(1);
    lcd_print("Alarm System!");

    vTaskDelay(pdMS_TO_TICKS(2000)); 


    // Main loop
    while (1) {
        update_leds();

        if (newly_armed) {
            newly_armed = false;
            lcd_set_cursor(0);
            lcd_print("SYSTEM ARMED!");
            lcd_set_cursor(1);
            lcd_print("");
        }
        if (newly_disarmed) {
            newly_disarmed = false;
            lcd_set_cursor(0);
            lcd_print("SYSTEM DISARMED!");
            lcd_set_cursor(1);
            lcd_print("");
        }
        if (newly_triggered) {
            newly_triggered = false;
            lcd_set_cursor(0);
            lcd_print("ALARM TRIGGERED!");
        }
        if (first_code_press) {
            first_code_press = false;
            lcd_set_cursor(0);
            lcd_print("Setting Code");
            lcd_set_cursor(1);
            lcd_print("");
        }
        if (second_code_press) {
            second_code_press = false;
            lcd_set_cursor(0);
            lcd_print("New Code Set!");
            lcd_set_cursor(1);
            lcd_print("");
        }
        if (code_button_pressed) {
            code_button_pressed = false;
            lcd_set_cursor(1);
            if (setting_code) {
                lcd_print_binary("Set: ", temp_code); 
            } else if (alarm_armed) {
                lcd_print_binary("Guess: ", guess_code);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}