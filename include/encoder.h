#ifndef __ENCODER_H__
#define __ENCODER_H__

#pragma once
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_attr.h" // <--- FUNDAMENTAL para IRAM_ATTR

class Encoder
{
public:
    volatile int32_t vueltas = 0;

    Encoder(gpio_num_t A_, gpio_num_t B_);
    void init();
    int32_t getRawValue();

    void IRAM_ATTR clear()
    {
        if (pcnt_unit)
            pcnt_unit_clear_count(pcnt_unit);
    }

    void IRAM_ATTR procesarIndice()
    {
        int32_t pulsos = getRawValue();
        // Si pulsos es > 0 íbamos hacia adelante, si < 0 hacia atrás, podemos ponerlo hasta 50 para evitar rebotes, aunque con el filtro de hardware no debería haberlos
        if (pulsos > 20)
        {
            vueltas = vueltas + 1;
        }
        else if (pulsos < -20)
        {
            vueltas = vueltas - 1;
        }
        clear();
    }

private:
    pcnt_unit_handle_t pcnt_unit = nullptr;
    pcnt_channel_handle_t pcnt_chan_a = nullptr;
    pcnt_channel_handle_t pcnt_chan_b = nullptr;
    gpio_num_t gpioA;
    gpio_num_t gpioB;
};

#endif //__ENCODER_H__