#include <stdio.h>
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_adc/adc_oneshot.h"
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

//ADC Handle
adc_oneshot_unit_handle_t adc1_handle;

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

// ----------------- Timer ISR -----------------

static bool timer_handler(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    timer_triggered = true;
    return false;
}

// ----------------- Setup Functions -----------------

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

// TODO: Implement setup_joystick.
//   1. Configure SW_PIN (the joystick's push-button) as an input with the
//      internal pull-up enabled, same as any other button this course.
//   2. Build an adc_oneshot_unit_init_cfg_t with unit_id = ADC_UNIT_1 and
//      create the unit with adc_oneshot_new_unit(&config, &adc1_handle).
//   3. Build ONE adc_oneshot_chan_cfg_t (bitwidth = ADC_BITWIDTH_DEFAULT,
//      atten = ADC_ATTEN_DB_12 for the full 0-3.3V range) and apply it to
//      BOTH the joystick's X and Y channels (ADC_CHANNEL_0 and
//      ADC_CHANNEL_3) with adc_oneshot_config_channel().
void setup_joystick() {
    // TODO: fill in
}
