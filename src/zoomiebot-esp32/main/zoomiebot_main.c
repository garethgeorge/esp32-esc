
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "esc.h"

// Useful docs
// - PWM: https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-reference/peripherals/pwm.html
// - https://embeddedexplorer.com/esp32-pwm-using-ledc-peripheral/

#define STACK_SIZE 1024

const int LED_PIN = 2;

ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .timer_num = LEDC_TIMER_0,
    .duty_resolution = LEDC_TIMER_13_BIT,
    .freq_hz = 50,
    .clk_cfg = LEDC_AUTO_CLK};

ledc_channel_config_t ledc_channel[1];

int ms_to_duty(int freq, int ms)
{
    int duty_cycle_ms = (1000 / freq);
    return (ms * 8191) / duty_cycle_ms;
}

void app_main(void)
{
    printf("Hello world!\n");

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ledc_channel[0].channel = LEDC_CHANNEL_0;
    ledc_channel[0].gpio_num = GPIO_NUM_27;
    ledc_channel[0].speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel[0].timer_sel = LEDC_TIMER_0;
    ledc_channel[0].intr_type = LEDC_INTR_DISABLE;
    ledc_channel[0].duty = 0;
    ledc_channel[0].hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[0]));

    for (int i = 0;; i += 1000)
    {
        int duty;
        duty = ms_to_duty(50, 1);
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
        vTaskDelay(pdMS_TO_TICKS(1000));
        duty = ms_to_duty(50, 2);
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed");
        return;
    }

    printf("%uMB %s flash\n", flash_size / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    for (int i = 10; i >= 0; i--)
    {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}