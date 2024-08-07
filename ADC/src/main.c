#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

// Define el canal ADC y el GPIO
#define ADC_CHANNEL ADC_CHANNEL_0
#define ADC_GPIO 6

void init_adc(adc_oneshot_unit_handle_t* adc_handle);
void read_adc(adc_oneshot_unit_handle_t adc_handle);

void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;
    init_adc(&adc_handle);

    while (1)
    {
        read_adc(adc_handle);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void init_adc(adc_oneshot_unit_handle_t* adc_handle)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc_handle, ADC_CHANNEL, &config));
}

void read_adc(adc_oneshot_unit_handle_t adc_handle)
{
    int adc_reading = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_reading));
    printf("ADC Reading: %d\n", adc_reading);
}
