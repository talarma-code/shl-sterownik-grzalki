#pragma once
#include "HeaterAPI.h"

class HeaterDirect : public HeaterAPI {
public:
    HeaterDirect(int pin);

    void turnOn() override;
    void turnOff() override;
    bool isOn() const override;

private:
    int _pin;
    bool _state;
};
