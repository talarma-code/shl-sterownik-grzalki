#include "PulsePowerMeter.h"
#include "driver/pcnt.h"

PulsePowerMeter::PulsePowerMeter(uint32_t pin) : _pin(pin) {}

void PulsePowerMeter::setup() {
    pcnt_config_t pcnt_config = {};
    pcnt_config.pulse_gpio_num = _pin;       // pin impulsów licznika
    pcnt_config.ctrl_gpio_num = PCNT_PIN_NOT_USED; // brak kierunku
    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = PCNT_UNIT_0;
    pcnt_config.pos_mode = PCNT_COUNT_INC; // liczenie w górę
    pcnt_config.neg_mode = PCNT_COUNT_DIS; // ignorowanie spadków
    pcnt_config.lctrl_mode = PCNT_MODE_KEEP;
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_config.counter_h_lim = 1000000;  // ograniczenie licznika
    pcnt_config.counter_l_lim = 0;

    gpio_set_pull_mode((gpio_num_t)_pin, GPIO_PULLUP_ONLY);
    gpio_set_direction((gpio_num_t)_pin, GPIO_MODE_INPUT);

    pcnt_unit_config(&pcnt_config);
    pcnt_counter_clear(PCNT_UNIT_0);
    pcnt_counter_resume(PCNT_UNIT_0);
}

uint32_t PulsePowerMeter::count() {
    int16_t count;
    pcnt_get_counter_value(PCNT_UNIT_0, &count);
    return count;
}