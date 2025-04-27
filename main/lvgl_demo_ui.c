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

void example_lvgl_demo_ui(lv_disp_t *disp, RingbufHandle_t ring_buffer)
{
    lv_obj_t *scr = lv_disp_get_scr_act(disp);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(label, "Hello Wokwi, ESP32 & LVGL.");
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(label, disp->driver->hor_res);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    // Test read from buffer
    size_t item_size;
    char *item = (char *)xRingbufferReceive(ring_buffer, &item_size, pdMS_TO_TICKS(1000));
    // Check received item
    if (item != NULL)
    {
        // Print item
        for (int i = 0; i < item_size; i++)
        {
            printf("%c", item[i]);
        }
        printf("\n");
        // Return Item
        vRingbufferReturnItem(ring_buffer, (void *)item);
    }

    lv_obj_t *timer_label = lv_label_create(lv_scr_act());
    lv_obj_align(timer_label, LV_ALIGN_BOTTOM_MID, 0, 0);
    for (int counter = 0;; counter++)
    {
        lv_label_set_text(timer_label, LV_SYMBOL_OK);
        // lv_label_set_text_fmt(timer_label, "Uptime: %d sec", counter);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
