#include <stdio.h>
#include <math.h>
#include "driver/adc.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADC_CHANNEL ADC1_CHANNEL_0 // Canal ADC 0 (GPIO36)
#define SAMPLE_COUNT 100
#define TIMER_INTERVAL_MS 1

static int samples[SAMPLE_COUNT]; // Array para almacenar las muestras del ADC
static int sample_index = 0; // Índice para el array de muestras
static esp_timer_handle_t periodic_timer; // Manejador del temporizador

// Función para calcular el valor RMS de las muestras almacenadas
void calculate_rms() {
    long sum_of_squares = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        sum_of_squares += samples[i] * samples[i]; // Suma de los cuadrados de las muestras
    }
    float mean_square = sum_of_squares / (float)SAMPLE_COUNT; // Promedio de los cuadrados
    float rms = sqrt(mean_square); // Raíz cuadrada del promedio para obtener el RMS
    printf("RMS Value (V): %f\n", rms * 3.3 / 4095); // Imprime el valor RMS en la consola serial (ajustado a voltios)
}

// Función que se llama en cada interrupción de 1ms
void adc_sampling_task(void* arg) {
    int raw_value = adc1_get_raw(ADC_CHANNEL); // Toma una muestra del ADC
    samples[sample_index++] = raw_value; // Guarda la muestra en el array

    // Si hemos tomado 100 muestras, calculamos el RMS
    if (sample_index >= SAMPLE_COUNT) {
        calculate_rms(); // Calcula el RMS
        sample_index = 0; // Reinicia el índice para nuevas muestras
    }
}

// Función principal
void app_main() {
    // Configuración del ADC
    adc1_config_width(ADC_WIDTH_BIT_12); // Configura el ADC a 12 bits
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_0); // Atenuación de 0dB para un rango de 0-3.3V

    // Crear un temporizador periódico para muestreo
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &adc_sampling_task, // Función a llamar en cada interrupción
        .name = "periodic_adc_sampling"
    };
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer)); // Crea el temporizador
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, TIMER_INTERVAL_MS * 1000)); // Inicia el temporizador con un intervalo de 1ms
}
