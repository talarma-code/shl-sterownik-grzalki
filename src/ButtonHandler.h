#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "ActiveQueueRef.h"
#include "ShlProtocolPacket.h"

class ButtonHandler {
public:
    enum class PressCategory : uint8_t {
        Short = 0,
        Long = 1
    };

    struct Event {
        PressCategory category;
    };

    using EventHandler = void (*)(const Event &event, void *context);

    ButtonHandler(uint8_t pin,
                  ActiveQueueRef<SystemMessage> queue,
                  uint32_t debounceMs = 30,
                  uint32_t shortThresholdMs = 1500,
                  bool activeLow = true);

    void begin();
    uint32_t lastPressDurationMs() const { return _lastPressDurationMs; }
    PressCategory lastPressCategory() const { return _lastPressCategory; }

    // Call this regularly from loop() to process button state changes
    void update();

private:
    static void IRAM_ATTR interruptHandler(void *arg);
    void handleInterrupt();
    void enqueueEvent(const Event &event);
    bool readRawState() const;
    PressCategory classifyDuration(uint32_t durationMs) const;
    

    uint8_t _pin;
    uint32_t _debounceMs;
    uint32_t _shortThresholdMs;
    bool _activeLow;
    ActiveQueueRef<SystemMessage> _queue;

    volatile bool _interruptFired = false;
    volatile uint32_t _stateChangeTimeMs = 0;
    volatile uint32_t _pressStartMs = 0;
    volatile uint32_t _lastPressDurationMs = 0;
    volatile PressCategory _lastPressCategory = PressCategory::Short;
    volatile enum State {
        Idle,       // Waiting for button press
        Pressing   // Button pressed, measuring duration
    } _state = Idle;

    const char* stateDebugString(ButtonHandler::State state);

    EventHandler _eventHandler = nullptr;
    void *_eventContext = nullptr;
};
