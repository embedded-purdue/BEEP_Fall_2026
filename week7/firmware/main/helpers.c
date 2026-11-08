#include <stdio.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "rom/ets_sys.h"
#include "esp_timer.h"
#include "helpers.h"

//I2C Bus Handles
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t lcd_handle;


// ----------------- LCD Helpers -----------------

//command send helper
static void lcd_send_cmd(uint8_t cmd) {
    uint8_t high_nibble = (cmd & 0xF0) | FLAG_RS_COMMAND | FLAG_BACKLIGHT_ON;
    uint8_t low_nibble  = ((cmd << 4) & 0xF0) | FLAG_RS_COMMAND | FLAG_BACKLIGHT_ON;

    uint8_t buffer[6] = {
        high_nibble, 
        high_nibble | FLAG_ENABLE, 
        high_nibble,
        low_nibble, 
        low_nibble | FLAG_ENABLE, 
        low_nibble
    };

    i2c_master_transmit(lcd_handle, buffer, 6, 100);
    
    if (cmd == 0x01 || cmd == 0x02) {
        ets_delay_us(2000); 
    } else {
        ets_delay_us(50);   
    }
}

//character send helper
static void lcd_send_char(uint8_t chr) {
    uint8_t high_nibble = (chr & 0xF0) | FLAG_RS_DATA | FLAG_BACKLIGHT_ON;
    uint8_t low_nibble  = ((chr << 4) & 0xF0) | FLAG_RS_DATA | FLAG_BACKLIGHT_ON;

    uint8_t buffer[6] = {
        high_nibble, 
        high_nibble | FLAG_ENABLE, 
        high_nibble,
        low_nibble, 
        low_nibble | FLAG_ENABLE, 
        low_nibble
    };

    i2c_master_transmit(lcd_handle, buffer, 6, 100);
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

//prints the given string and fills out the line with whitespace
void lcd_print(const char *str) {
    uint8_t count = 0;
    
    while (*str && count < 16) {
        lcd_send_char((uint8_t)(*str));
        str++;
        count++;
    }
    
    while (count < 16) {
        lcd_send_char(' ');
        count++;
    }
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

//Setup I2C and Run the LCD Init Sequence
void setup_lcd() {
    //I2C Bus Config
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 0, 
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_new_master_bus(&i2c_mst_config, &bus_handle);

    //Add Device with LCD_ADDR to the Bus Config
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LCD_ADDR,
        .scl_speed_hz = 100000,
    };
    i2c_master_bus_add_device(bus_handle, &dev_cfg, &lcd_handle);

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

// TODO: Implement setup_ultrasonic.
//   - TRIG_PIN: reset it (gpio_reset_pin), set it as an OUTPUT, and drive it
//     low to start (it idles low, and only pulses high to trigger a reading).
//   - ECHO_PIN: reset it and set it as an INPUT (the sensor drives this pin,
//     we just read it).
void setup_ultrasonic() {
    // TODO: fill in
}
