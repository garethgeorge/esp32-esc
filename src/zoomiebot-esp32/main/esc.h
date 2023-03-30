#ifndef ESC_H
#define ESC_H

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

typedef struct ESC
{
    ledc_timer_config_t timer_config;
    ledc_channel_config_t channel_config;
    int pin;
    int duty;
} ESC;

extern void esc_setup(ESC *esc, int timer, int channel, int pin, int freq);
extern void esc_set_duty(ESC *esc, int duty);
extern void esc_set_pulse_duration(ESC *esc, int pulse_duration);
extern int esc_get_duty(ESC *esc);

#endif