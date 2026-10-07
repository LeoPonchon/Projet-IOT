#pragma once

#include <Arduino.h>

namespace Config {

constexpr uint8_t LDR_PIN = 32;
constexpr uint8_t THERMISTOR_PIN = 33;
constexpr uint8_t LED_PIN = 25;
constexpr uint8_t BUTTON_NEXT = 35;
constexpr uint8_t BUTTON_PREVIOUS = 0;
constexpr uint8_t BACKLIGHT_PIN = 4;

constexpr float FIXED_RESISTOR = 10000.0f;
constexpr float NOMINAL_RESISTANCE = 10000.0f;
constexpr float NOMINAL_TEMPERATURE = 25.0f;
constexpr float B_COEFFICIENT = 3950.0f;

constexpr int DEFAULT_THRESHOLD_ON = 1500;
constexpr int DEFAULT_THRESHOLD_OFF = 1900;

}
