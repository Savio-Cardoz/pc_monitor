/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "fan.h"

void example_lvgl_demo_ui(lv_disp_t *disp, RingbufHandle_t ring_buffer)
{
    // lv_obj_t *scr = lv_disp_get_scr_act(disp);
    // lv_obj_t *label = lv_label_create(scr);
    // lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    // lv_label_set_text(label, "Hello Wokwi, ESP32 & LVGL.");
    // /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    // lv_obj_set_width(label, disp->driver->hor_res);
    // lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    char *disp_data_buf = malloc(sizeof(char) * 50);
    char parameter_array[6][16];
    const char *delimiters = ",";

    lv_obj_t *timer_label = lv_label_create(lv_scr_act());
    lv_obj_align(timer_label, LV_ALIGN_TOP_LEFT, 0, 0);

    while (1)
    {
        memset(disp_data_buf, 0, sizeof(char) * 50);

        // Test read from buffer
        size_t item_size;
        char *item = (char *)xRingbufferReceive(ring_buffer, &item_size, pdMS_TO_TICKS(1000));
        // Check received item
        if (item != NULL)
        {

            // Get the first token
            char *token = strtok(item, delimiters);

            int param_number = 0;
            // Loop through the tokens
            while (token != NULL)
            {
                printf("%s - %d\n", token, strlen(token)); // Print the token
                memcpy(parameter_array[param_number], token, strlen(token));
                parameter_array[param_number++][strlen(token)] = '\0';
                token = strtok(NULL, delimiters); // Get the next token
            }

            sprintf(disp_data_buf, "CPU: %s%% \n", parameter_array[1]);
            sprintf(disp_data_buf + strlen(disp_data_buf), "GPU T: %s \n", parameter_array[2]);
            sprintf(disp_data_buf + strlen(disp_data_buf), "GPU: %s%% \n", parameter_array[3]);
            sprintf(disp_data_buf + strlen(disp_data_buf), "Fan: %d%% ", atoi(parameter_array[3]));

            update_fan_duty_cycle(100 - atoi(parameter_array[3])); // Substract from 100 to get the duty cycle as the fan is inverted
            printf("Duty set: %d\n", atoi(parameter_array[3]));

            // Return Item
            vRingbufferReturnItem(ring_buffer, (void *)item);
            lv_label_set_text_fmt(timer_label, "%s", disp_data_buf);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
