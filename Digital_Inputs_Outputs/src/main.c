#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define LED_GPIO 47
#define BT 17 

void app_main() 
{
     //Pin del LED como salida
     gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
     
     //Pin del botón como entrada con pull-down
     gpio_set_direction(BT, GPIO_MODE_INPUT);
     gpio_set_pull_mode(BT, GPIO_PULLDOWN_ONLY);

     while(1)

     {

        int state_bt = gpio_get_level(BT);
        printf("%d\n", state_bt);
        if(state_bt){
            gpio_set_level(LED_GPIO, 1);
           // printf("\nBotón presionado, LED ON");
        }
        else{
            gpio_set_level(LED_GPIO, 0);
           // printf("\nButón presionado, LED OFF");
        }
         
        vTaskDelay(200 / portTICK_PERIOD_MS);
     }


}