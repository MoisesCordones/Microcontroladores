#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

//#define LED_GPIO 47
#define BT 17

int display[] = {6, 7, 18, 16, 15, 4, 5};

char number[10][7] = {
    {0, 0, 0, 0, 0, 0, 1}, //0
    {1, 0, 0, 1, 1, 1, 1}, //1
    {0, 0, 1, 0, 0, 1, 0}, //2
    {0, 0, 0, 0, 1, 1, 0}, //3
    {1, 0, 0, 1, 1, 0, 0}, //4
    {0, 1, 0, 0, 1, 0, 0}, //5
    {0, 1, 0, 0, 0, 0, 0}, //6
    {0, 0, 0, 1, 1, 1, 1}, //7
    {0, 0, 0, 0, 0, 0, 0}, //8
    {0, 0, 0, 0, 1, 0, 0}, //9
};

void init_hw(void);

void app_main() 
{
    init_hw();
    int  cnt = 0;
    for(int i=0; i<7; i++) 
    {
         gpio_set_level(display[i], number[0][i]);
    }


    while (1)
    {
        int state_bt = gpio_get_level(BT);
        printf("Botón: %d, Contador: %d\n", state_bt, cnt); // Verificar estado

        for(int i=0; i<7; i++) 
        {
         gpio_set_level(display[i], number[cnt][i]);
    
        }

        cnt = (state_bt && cnt < 10) ? (cnt +1) : cnt;
        cnt = cnt > 9 ? 0 : cnt;


        vTaskDelay(200 / portTICK_PERIOD_MS);
        /* code */
    }
    


}

void init_hw(void)
{
    gpio_config_t io_config;

    io_config.mode = GPIO_MODE_INPUT;
    io_config.pin_bit_mask = (1ULL << BT);
    io_config.pull_down_en = GPIO_PULLDOWN_ONLY;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_config);

    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pin_bit_mask = (1ULL << display[0]) |
                             (1ULL << display[1]) |
                             (1ULL << display[2]) |
                             (1ULL << display[3]) |
                             (1ULL << display[4]) |
                             (1ULL << display[5]) |
                             (1ULL << display[6]);
                            
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_config);
}