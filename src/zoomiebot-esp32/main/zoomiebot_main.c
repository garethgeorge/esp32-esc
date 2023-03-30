
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_wifi.h"

#include "esc.h"

// Useful docs
// - PWM: https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/api-reference/peripherals/pwm.html
// - https://embeddedexplorer.com/esp32-pwm-using-ledc-peripheral/

#define WIFI_SSID "Zoomiebot"
#define WIFI_PASS "z00mbot"
#define STACK_SIZE 1024

const int LED_PIN = 2;

void calibrartion_sequence(ESC *esc)
{
    printf("Throttling to max to trigger calibration.\n");
    esc_set_pulse_duration(&esc, 2000);
    vTaskDelay(6000 / portTICK_PERIOD_MS);
    printf("Throttling to min to calibrate low end\n");
    esc_set_pulse_duration(&esc, 1000);
}

void app_main(void)
{
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();

    ESC esc;
    esc_setup(&esc, LEDC_TIMER_0, LEDC_CHANNEL_0, GPIO_NUM_27, 50);

    printf("ESC OUTPUT ON PIN 27\n");

    printf("Set throttle to 0 for startup sequence\n");
    esc_set_pulse_duration(&esc, 1000);
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    for (int i = 0; i <= 1000; i += 10)
    {
        printf("Gradually ramping throttle, PWM pulse width microseconds: %d\n", 1000 + i);
        esc_set_pulse_duration(&esc, 1000 + i);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    printf("Done throttling up!");

    while (true)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
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