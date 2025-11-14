#include "HeaterDirect.h"
#include <Arduino.h> // dla ESP32/ESP-NOW, PWM, pinMode, digitalWrite

HeaterDirect::HeaterDirect(int pin) : _pin(pin), _state(false) {
    pinMode(_pin, OUTPUT);
}

void HeaterDirect::turnOn() {
    _state = true;
    digitalWrite(_pin, HIGH);
}

void HeaterDirect::turnOff() {
    _state = false;
    digitalWrite(_pin, LOW);
}

bool HeaterDirect::isOn() const {
    return _state;
}