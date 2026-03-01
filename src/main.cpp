#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "encoder.h"

#define PIN_INDEX GPIO_NUM_21
#define PIN_MOT_A GPIO_NUM_22
#define PIN_MOT_B GPIO_NUM_23

Encoder A(GPIO_NUM_18, GPIO_NUM_19);

void IRAM_ATTR index_handler(void *arg)
{
    ((Encoder *)arg)->procesarIndice();
}

extern "C" void app_main()
{

    // Config Index
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = (1ULL << PIN_INDEX);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    A.init();

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_INDEX, index_handler, (void *)&A);

    // --- Configuración de Control ---
    float objetivo_grados = 720.0;  // Ejemplo: Queremos que gire 2 vueltas exactas
    const float margen_error = 2.0; // Margen de +/- 2 grados para detenerse

    // Configuración de los pines del motor como salida (si no lo hiciste antes)
    gpio_set_direction(PIN_MOT_A, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_MOT_B, GPIO_MODE_OUTPUT);

    while (1)
    {
        int32_t p = A.getRawValue();
        int32_t v = A.vueltas;
        float grados_actuales = (v * 360.0f) + (p * 360.0f / 400.0f);

        // --- LÓGICA DE CONTROL ---
        if (grados_actuales < (objetivo_grados - margen_error))
        {
            // El motor está por detrás de la meta -> Mover adelante
            gpio_set_level(PIN_MOT_A, 1);
            gpio_set_level(PIN_MOT_B, 0);
        }
        else if (grados_actuales > (objetivo_grados + margen_error))
        {
            // El motor se pasó de la meta -> Mover atrás
            gpio_set_level(PIN_MOT_A, 0);
            gpio_set_level(PIN_MOT_B, 1);
        }
        else
        {
            // Estamos en el objetivo -> Frenar motor
            gpio_set_level(PIN_MOT_A, 0);
            gpio_set_level(PIN_MOT_B, 0);

            printf("Vueltas: %ld | Pulsos: %ld | Total: %.2f deg\n", v, p, grados_actuales);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}
