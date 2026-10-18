#include <stdio.h>
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "rom/ets_sys.h"
#include "esp_timer.h"
#include "helpers.h"

//Timer Handle
gptimer_handle_t timer_handle;

//Timer ISR Flag
volatile bool timer_triggered = false;

//I2C Bus Handles
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t lcd_handle;

// ----------------- Timer ISR -----------------

static bool timer_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    timer_triggered = true;
    return false;
}

// ----------------- I2C LCD Helpers -----------------

// TODO: Implement lcd_send_cmd.
// high_nibble/low_nibble are already computed for you below. You need to
// build a 6-byte buffer and transmit it over I2C:
//   [high_nibble, high_nibble|FLAG_ENABLE, high_nibble,
//    low_nibble,  low_nibble|FLAG_ENABLE,  low_nibble]
// This "settle, pulse enable, settle again" pattern is what shifts the
// nibble onto the LCD's internal 4-bit bus. Send it with
// i2c_master_transmit(lcd_handle, buffer, 6, 100).
static void lcd_send_cmd(uint8_t cmd) {
    uint8_t high_nibble = (cmd & 0xF0) | FLAG_RS_COMMAND | FLAG_BACKLIGHT_ON;
    uint8_t low_nibble  = ((cmd << 4) & 0xF0) | FLAG_RS_COMMAND | FLAG_BACKLIGHT_ON;

    // TODO: build the 6-byte buffer and call i2c_master_transmit()

    if (cmd == 0x01 || cmd == 0x02) {
        ets_delay_us(2000); 
    } else {
        ets_delay_us(50);   
    }
}

// TODO: Implement lcd_send_char. Same structure as lcd_send_cmd above, but
// using FLAG_RS_DATA instead of FLAG_RS_COMMAND (already reflected in the
// nibble calculation below) since we're sending a character to be displayed,
// not a command to the controller.
static void lcd_send_char(uint8_t chr) {
    uint8_t high_nibble = (chr & 0xF0) | FLAG_RS_DATA | FLAG_BACKLIGHT_ON;
    uint8_t low_nibble  = ((chr << 4) & 0xF0) | FLAG_RS_DATA | FLAG_BACKLIGHT_ON;

    // TODO: build the 6-byte buffer and call i2c_master_transmit()

    ets_delay_us(50); 
}

//sets cursor to a row, 0=top, 1=bottom
void lcd_set_cursor(bool row) {
    if (row) {
        lcd_send_cmd(0x80 | 0x40);
    }
    else {
        lcd_send_cmd(0x80 | 0x00);
    }
}

// TODO: Implement lcd_print.
// This does a full line rewrite every call, so it needs two while loops:
//   1. Print characters from `str` one at a time (using lcd_send_char) until
//      either you hit the null terminator or you've printed 16 characters
//      (whichever comes first). Remember `str` is a pointer - dereference it
//      to get the current char, then increment the pointer to advance.
//   2. Keep printing ' ' (space) characters with lcd_send_char until you've
//      printed a full 16 characters total, so any leftover text from the
//      previous line gets overwritten with blanks.
void lcd_print(const char *str) {
    // TODO: fill in
}

//binary printer for second line code printing
void lcd_print_binary(const char* prefix, uint8_t val) {
    char buffer[17];
    
    snprintf(buffer, sizeof(buffer), "%s%c%c%c%c%c%c%c%c", prefix,
        (val & 0x80) ? '1' : '0', (val & 0x40) ? '1' : '0',
        (val & 0x20) ? '1' : '0', (val & 0x10) ? '1' : '0',
        (val & 0x08) ? '1' : '0', (val & 0x04) ? '1' : '0',
        (val & 0x02) ? '1' : '0', (val & 0x01) ? '1' : '0');
        
    lcd_print(buffer);
}

//nibble sender for init sequence
static void lcd_init_nibble(uint8_t nibble) {
    uint8_t data_setup = nibble | FLAG_BACKLIGHT_ON;
    uint8_t data_enable = data_setup | FLAG_ENABLE;
    
    // Send Setup (EN=0), Pulse (EN=1), Latch (EN=0) in one transaction
    uint8_t buffer[3] = {data_setup, data_enable, data_setup};
    i2c_master_transmit(lcd_handle, buffer, 3, 100); // 100ms timeout
}

// ----------------- Setup Functions -----------------
void pwm_setup() {
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT, // duty cycle values from 0-1024
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 10000, //10KHz
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_channel_config_t red_config = {
        .gpio_num = RED_LED_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0 //phase shift/offset
    };
    ledc_channel_config_t green_config = {
        .gpio_num = GREEN_LED_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config_t blue_config = {
        .gpio_num = BLUE_LED_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_2,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };

    ledc_timer_config(&timer_config);
    ledc_channel_config(&red_config);
    ledc_channel_config(&green_config);
    ledc_channel_config(&blue_config);
}


void timer_setup(uint32_t res_hz, uint64_t alarm_hz) {
    gptimer_config_t timer_config = {
        .clk_src = SOC_MOD_CLK_APB,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = res_hz,
        .intr_priority = 0
    };
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = alarm_hz, 
        .reload_count = 0, 
        .flags.auto_reload_on_alarm = true
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_handler 
    };

    gptimer_new_timer(&timer_config, &timer_handle);
    gptimer_set_alarm_action(timer_handle, &alarm_config);
    gptimer_register_event_callbacks(timer_handle,&cbs,NULL);
    gptimer_enable(timer_handle);
    gptimer_start(timer_handle);
}

// TODO: Implement setup_lcd.
//   1. Build an i2c_master_bus_config_t: clk_source = I2C_CLK_SRC_DEFAULT,
//      i2c_port = 0, scl_io_num = I2C_SCL_PIN, sda_io_num = I2C_SDA_PIN,
//      glitch_ignore_cnt = 7, flags.enable_internal_pullup = true. Create the
//      bus with i2c_new_master_bus(&config, &bus_handle).
//   2. Build an i2c_device_config_t: dev_addr_length = I2C_ADDR_BIT_LEN_7,
//      device_address = LCD_ADDR, scl_speed_hz = 100000. Add the device with
//      i2c_master_bus_add_device(bus_handle, &config, &lcd_handle).
// The rest of the LCD-specific init sequence below is provided for you -
// it's a fixed hardware quirk of this particular LCD backpack chip, not a
// general I2C concept, so there's nothing to learn by re-deriving it.
void setup_lcd() {
    //I2C Bus Config
    // TODO: fill in the bus config struct and call i2c_new_master_bus()


    //Add Device with LCD_ADDR to the Bus Config
    // TODO: fill in the device config struct and call i2c_master_bus_add_device()


    ets_delay_us(50000); 
    
    //INIT SEQUENCE

    //known state reset
    uint8_t data = 0x00 | FLAG_BACKLIGHT_ON;
    i2c_master_transmit(lcd_handle, &data, 1, 100);
    ets_delay_us(1000);

    //switching from 8 to 4bit mode
    lcd_init_nibble(0x30); 
    ets_delay_us(4500);
    lcd_init_nibble(0x30); 
    ets_delay_us(4500);
    lcd_init_nibble(0x30); 
    ets_delay_us(120);
    lcd_init_nibble(0x20); 
    ets_delay_us(50);

    lcd_send_cmd(0x28); // 4-bit, 2 Line, 5x8 Dots
    lcd_send_cmd(0x0C); // Display ON, Cursor OFF
    lcd_send_cmd(0x01); // Clear Display
    lcd_send_cmd(0x06); // Increment Cursor
    lcd_send_cmd(0x02); // Return Home
}
