#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "helpers.h"

const char* TAG = "MAIN";

//Game State
char row_top[17] =    "                ";
char row_bottom[17] = "                ";
int ship_y = 0;
int score = 0;
bool game_over = false;

void app_main(void) {
    setup_lcd();
    setup_joystick();
    timer_setup(1000000, 200000);
    
    int adc_x = 2048;
    int adc_y = 2048;

    // These implement the EMA filter described in 6.1.4 of the README.
    // smoothed_y holds the running smoothed reading (start at 2048, the
    // midpoint of the 12-bit ADC range), alpha controls how aggressively new
    // readings pull that value around.
    float smoothed_y = 2048.0f;
    float alpha = 0.3f;        

    while (1) {
        // TODO: Read the joystick's X and Y axes with adc_oneshot_read()
        // into adc_x and adc_y (X is ADC_CHANNEL_0, Y is ADC_CHANNEL_3).


        // TODO: Apply the EMA smoothing formula from 6.1.4 to smoothed_y:
        //   smoothed_y = (alpha * adc_y) + ((1.0f - alpha) * smoothed_y);


        // TODO: Decide the ship's row from the smoothed reading. Use the
        // thresholds from the README: if smoothed_y > 3200, ship_y = 1
        // (bottom row); if smoothed_y < 800, ship_y = 0 (top row). Leave
        // ship_y unchanged in the dead zone between those thresholds so the
        // ship doesn't jitter when the stick is near center.


        //button press check
        if (game_over && gpio_get_level(SW_PIN) == 0) {
            //reset game state
            game_over = false;
            score = 0;

            //clear screen
            snprintf(row_top, 17, "                ");
            snprintf(row_bottom, 17, "                ");

            lcd_set_cursor(false);
            lcd_print(row_top);
            lcd_set_cursor(true);
            lcd_print(row_bottom);
        }

        //game ticks
        if (timer_triggered) {
            timer_triggered = false; 

            if (!game_over) {
                //shift the rocks
                for (int i = 0; i < 15; i++) {
                    row_top[i] = row_top[i+1];
                    row_bottom[i] = row_bottom[i+1];
                }
                
                //new rock spawning
                row_top[15] = ' ';
                row_bottom[15] = ' ';
                
                static int rock_cooldown = 1; 
                if (rock_cooldown > 0) {
                    rock_cooldown--;
                } else {
                    //70% chance to spawn a rock when the cooldown is 0
                    if (rand() % 10 < 7) { 
                        if (rand() % 2 == 0) {
                            row_top[15] = '*';
                        } else {
                            row_bottom[15] = '*';
                        }
                        //reset cooldown
                        rock_cooldown = 1; 
                    }
                }

                //detect collisions with rocks
                if ((ship_y == 0 && row_top[1] == '*') || 
                    (ship_y == 1 && row_bottom[1] == '*')) {
                    game_over = true;
                } else {
                    score++;
                }

                //update ship location
                char temp_top = row_top[1];
                char temp_bottom = row_bottom[1];
                row_top[1] = (ship_y == 0) ? '>' : row_top[1];
                row_bottom[1] = (ship_y == 1) ? '>' : row_bottom[1];

                //update screen
                lcd_set_cursor(false);
                lcd_print(row_top);
                lcd_set_cursor(true);
                lcd_print(row_bottom);

                row_top[1] = temp_top;
                row_bottom[1] = temp_bottom;
            } 
            else { //game over screen
                lcd_set_cursor(false);
                lcd_print("    CRASHED!    ");
                
                char score_str[17];
                snprintf(score_str, sizeof(score_str), "Score: %d", score);
                lcd_set_cursor(true);
                lcd_print(score_str);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
