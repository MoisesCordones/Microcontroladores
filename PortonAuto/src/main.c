#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Definición de los pines GPIO
#define GPIO_MOTOR_ABRIENDO 18
#define GPIO_MOTOR_CERRANDO 19
#define GPIO_LUZ_PILOTO 21
#define GPIO_LUZ_EMERGENCIA 22
#define GPIO_LIMIT_SWITCH_ABIERTO 23
#define GPIO_LIMIT_SWITCH_CERRADO 25
#define GPIO_SENSOR_PROXIMIDAD 26
#define GPIO_COMANDO_ABRIR 27
#define GPIO_COMANDO_CERRAR 32

// Estados del portón
typedef enum {
    ESTADO_CERRADO,
    ESTADO_ABRIENDO,
    ESTADO_ABIERTO,
    ESTADO_CERRANDO,
    ESTADO_EMERGENCIA
} EstadoPorton;

EstadoPorton estado_actual = ESTADO_CERRADO;

// Función para configurar los GPIOs
void setup_gpio() {
    // Configuración de los GPIOs para el motor
    gpio_set_direction(GPIO_MOTOR_ABRIENDO, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_MOTOR_CERRANDO, GPIO_MODE_OUTPUT);

    // Configuración de los GPIOs para las luces
    gpio_set_direction(GPIO_LUZ_PILOTO, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_LUZ_EMERGENCIA, GPIO_MODE_OUTPUT);

    // Configuración de los GPIOs para los limit switches
    gpio_set_direction(GPIO_LIMIT_SWITCH_ABIERTO, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_LIMIT_SWITCH_CERRADO, GPIO_MODE_INPUT);

    // Configuración del sensor de proximidad
    gpio_set_direction(GPIO_SENSOR_PROXIMIDAD, GPIO_MODE_INPUT);

    // Configuración de pull-up para los botones
    gpio_set_direction(GPIO_COMANDO_ABRIR, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_COMANDO_ABRIR, GPIO_PULLUP_ONLY);

    gpio_set_direction(GPIO_COMANDO_CERRAR, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_COMANDO_CERRAR, GPIO_PULLUP_ONLY);
}

// Función para manejar la máquina de estados del portón
void maquina_estados_porton() {
    switch (estado_actual) {
        case ESTADO_CERRADO:
            if (gpio_get_level(GPIO_COMANDO_ABRIR) == 0) {
                estado_actual = ESTADO_ABRIENDO;
                gpio_set_level(GPIO_MOTOR_ABRIENDO, 1);
            }
            break;

        case ESTADO_ABRIENDO:
            if (gpio_get_level(GPIO_LIMIT_SWITCH_ABIERTO) == 0) {
                estado_actual = ESTADO_ABIERTO;
                gpio_set_level(GPIO_MOTOR_ABRIENDO, 0);
            }
            break;

        case ESTADO_ABIERTO:
            if (gpio_get_level(GPIO_COMANDO_CERRAR) == 0) {
                estado_actual = ESTADO_CERRANDO;
                gpio_set_level(GPIO_MOTOR_CERRANDO, 1);
            }
            break;

        case ESTADO_CERRANDO:
            if (gpio_get_level(GPIO_LIMIT_SWITCH_CERRADO) == 0) {
                estado_actual = ESTADO_CERRADO;
                gpio_set_level(GPIO_MOTOR_CERRANDO, 0);
            }
            break;

        case ESTADO_EMERGENCIA:
            gpio_set_level(GPIO_MOTOR_ABRIENDO, 0);
            gpio_set_level(GPIO_MOTOR_CERRANDO, 0);
            gpio_set_level(GPIO_LUZ_EMERGENCIA, 1);
            break;
    }
}

void app_main(void) {
    setup_gpio();  // Configuración de GPIOs

    while(1) {
        maquina_estados_porton();  // Actualizar estado del portón
        vTaskDelay(pdMS_TO_TICKS(100));  // Pequeña espera entre iteraciones
    }
}
