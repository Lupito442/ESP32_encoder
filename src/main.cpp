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

    while (1)
    {
        int32_t p = A.getRawValue();
        int32_t v = A.vueltas;
        float grados_vuelta_actual = (p * 360.0f) / 400.0f;
        float grados_totales = (v * 360.0f) + grados_vuelta_actual;

        printf("Vueltas: %ld | Pulsos: %ld | Total: %.2f grados\n", v, p, grados_totales);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
