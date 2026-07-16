#pragma once

#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

class Logger {
public:
    enum class Level : uint8_t {
        None = 0,
        Error = 1,
        Info = 2,
        Debug = 3
    };

    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    void begin(Stream* output, Level level = Level::Info) {
        _output = output;
        _level = level;
    }

    void setLevel(Level level) {
        _level = level;
    }

    Level level() const {
        return _level;
    }

    void log(Level level, const char* tag, const char* fmt, ...) {
        if (level > _level || _output == nullptr) {
            return;
        }

        char buffer[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);

        _output->print("[");
        _output->print(tag);
        _output->print("] ");
        _output->println(buffer);
    }

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Stream* _output = nullptr;
    Level _level = Level::Info;
};

#define LOG_INFO(...) ::Logger::instance().log(::Logger::Level::Info, "INFO", __VA_ARGS__)
#define LOG_DEBUG(...) ::Logger::instance().log(::Logger::Level::Debug, "DEBUG", __VA_ARGS__)
#define LOG_ERROR(...) ::Logger::instance().log(::Logger::Level::Error, "ERROR", __VA_ARGS__)
