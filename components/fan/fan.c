#include "fan.h"

#define PWM_RESOLUTION (1 << TIMER_RESOLUTION)
#define EXAMPLE_PCNT_HIGH_LIMIT (6000)
#define EXAMPLE_PCNT_LOW_LIMIT (-6000)
#define FAN_TAC_INPUT_GPIO (4)

static const char *TAG = "fan";

static ledc_channel_config_t ledc_channel = {
    .channel = LEDC_HS_CH0_CHANNEL,
    .duty = 0,
    .gpio_num = LEDC_HS_CH0_GPIO,
    .speed_mode = LEDC_HS_MODE,
    .hpoint = 0,
    .timer_sel = LEDC_HS_TIMER,
    .flags.output_invert = 0,
};

static pcnt_unit_handle_t pcnt_unit = NULL;

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

    ESP_LOGI(TAG, "install pcnt unit");
    pcnt_unit_config_t unit_config = {
        .high_limit = EXAMPLE_PCNT_HIGH_LIMIT,
        .low_limit = EXAMPLE_PCNT_LOW_LIMIT,
    };

    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    // ESP_LOGI(TAG, "set glitch filter");
    // pcnt_glitch_filter_config_t filter_config = {
    //     .max_glitch_ns = 1000,
    // };
    // ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_LOGI(TAG, "install pcnt channels");
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = FAN_TAC_INPUT_GPIO,
    };

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 10000, // Set the maximum glitch width in nanoseconds
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_INCREASE));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

void update_fan_duty_cycle(uint8_t duty_percentage)
{
    int new_duty = PWM_RESOLUTION * ((float)duty_percentage / 100.0);
    ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, new_duty);
    ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);
    printf("Duty set: %d\n", new_duty);
}

void get_pulse_count(int *pulse_count)
{
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, pulse_count));
    pcnt_unit_clear_count(pcnt_unit);
    printf("Pulse count: %d\n", *pulse_count);
}