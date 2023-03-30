#include "esc.h"

static int ms_to_duty(int freq, int ms)
{
    int duty_cycle_ms = (1000 / freq);
    return (ms * 8191) / duty_cycle_ms;
}

extern void esc_setup(ESC *esc, int timer, int channel, int pin, int freq)
{
    // TODO: refactor, timers can be shared by channels if they have the same
    // frequency and resolution
    esc->timer_config.speed_mode = LEDC_LOW_SPEED_MODE;
    esc->timer_config.timer_num = timer;
    esc->timer_config.duty_resolution = LEDC_TIMER_13_BIT;
    esc->timer_config.freq_hz = freq;
    esc->timer_config.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&esc->timer_config));

    esc->channel_config.channel = channel;
    esc->channel_config.gpio_num = pin;
    esc->channel_config.speed_mode = LEDC_LOW_SPEED_MODE;
    esc->channel_config.timer_sel = timer;
    esc->channel_config.intr_type = LEDC_INTR_DISABLE;
    esc->channel_config.duty = 0;
    esc->channel_config.hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&esc->channel_config));
}

extern void esc_set_duty(ESC *esc, int duty)
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, esc->channel_config.channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, esc->channel_config.channel));
    esc->duty = duty;
}

extern void esc_set_pulse_duration(ESC *esc, int pulse_duration)
{
    int duty = ms_to_duty(esc->timer_config.freq_hz, pulse_duration);
    esc_set_duty(esc, duty);
}