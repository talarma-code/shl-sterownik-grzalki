#include "ButtonHandler.h"
#include "Log.h"

const char* ButtonHandler::stateDebugString(ButtonHandler::State state) {
    switch (state) {
        case ButtonHandler::State::Idle:
            return "Idle";
        case ButtonHandler::State::Pressing:
            return "Pressing";
        default:
            return "Unknown";
    }
}

ButtonHandler::ButtonHandler(uint8_t pin,
                             ActiveQueueRef<SystemMessage> queue,    
                             uint32_t debounceMs,
                             uint32_t shortThresholdMs,
                             bool activeLow)
    : _pin(pin),
      _queue(queue),
      _debounceMs(debounceMs),
      _shortThresholdMs(shortThresholdMs),
      _activeLow(activeLow) {}

void ButtonHandler::begin() {
    pinMode(_pin, INPUT_PULLUP);

    _state = Idle;
    _stateChangeTimeMs = millis();
    _pressStartMs = 0;

    attachInterruptArg(digitalPinToInterrupt(_pin), interruptHandler, this, CHANGE);
}

void ButtonHandler::interruptHandler(void *arg) {
    auto *self = static_cast<ButtonHandler *>(arg);
    self->handleInterrupt();
}

void ButtonHandler::handleInterrupt() {
    // ISR should be as minimal as possible to avoid watchdog timeout
    // Just record the edge and let update() handle the logic
    _interruptFired = true;
}

void ButtonHandler::update() {
    if (!_interruptFired) {
        return;
    }
    _interruptFired = false;

    const uint32_t now = millis();
    const bool isButtonPressed = readRawState();

    switch (_state) {
        case Idle:
            // Waiting for button press
            if (isButtonPressed) {  // Button pressed
                _state = Pressing;
                _pressStartMs = now;
                _stateChangeTimeMs = now;
                LOG_DEBUG("Pressing (press started at %u)", now);
            }
            break;

        case Pressing:
            if (!isButtonPressed) {  
                _state = Idle;
                const uint32_t durationMs = now - _pressStartMs;
                _lastPressDurationMs = durationMs;
                _lastPressCategory = classifyDuration(durationMs);

                Event event{};
                event.category = _lastPressCategory;
                enqueueEvent(event);

                _stateChangeTimeMs = now;
                LOG_DEBUG("Idle (event generated, duration=%u ms, category=%d)", 
                         durationMs, static_cast<int>(_lastPressCategory));

            }
            // else: stay in Pressing (button still held or noise)
            break;
    }
}

void ButtonHandler::enqueueEvent(const Event &event) {
    //TODO add kode to send for required queue, but for now we will just log the event
    LOG_DEBUG("ButtonHandler::enqueueEvent - category=%d", static_cast<int>(event.category));

    SystemMessage msg;
    msg.type = SystemMessageType::TimerEvent;
    _queue.send(msg);
}

bool ButtonHandler::readRawState() const {
    const bool state = digitalRead(_pin);
    return _activeLow ? !state : state;
}

ButtonHandler::PressCategory ButtonHandler::classifyDuration(uint32_t durationMs) const {
    if (durationMs < _shortThresholdMs) {
        return PressCategory::Short;
    }

    return PressCategory::Long;
}
