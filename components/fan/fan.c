#include "fan.h"

#define PWM_RESOLUTION (1 << TIMER_RESOLUTION)

static ledc_channel_config_t ledc_channel = {
    .channel = LEDC_HS_CH0_CHANNEL,
    .duty = 0,
    .gpio_num = LEDC_HS_CH0_GPIO,
    .speed_mode = LEDC_HS_MODE,
    .hpoint = 0,
    .timer_sel = LEDC_HS_TIMER,
    .flags.output_invert = 0,
};

void init_fan_control()
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_9_BIT, // resolution of PWM duty
        .freq_hz = 25000,                    // frequency of PWM signal
        .clk_cfg = LEDC_AUTO_CLK,            // Auto select the source clock
        .speed_mode = LEDC_HS_MODE,
        .timer_num = LEDC_HS_TIMER,
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config(&ledc_channel);
}

void update_fan_duty_cycle(uint8_t duty_percentage)
{
    int new_duty = PWM_RESOLUTION * ((float)duty_percentage / 100.0);
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, new_duty);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
    printf("Duty set: %d\n", new_duty);
}