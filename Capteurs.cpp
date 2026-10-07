#include "Capteurs.h"

#include <math.h>

#include "Config.h"

namespace {

Mesures mesures = {0, 0, NAN};

int lireMoyenne(uint8_t broche) {
  long somme = 0;

  for (int i = 0; i < 16; i++) {
    somme += analogRead(broche);
    delay(2);
  }

  return somme / 16;
}

float convertirTemperature(int valeurAdc) {
  if (valeurAdc <= 0 || valeurAdc >= 4095) {
    return NAN;
  }

  float resistanceNtc =
    Config::FIXED_RESISTOR * (4095.0f / valeurAdc - 1.0f);

  float temperatureKelvin = resistanceNtc / Config::NOMINAL_RESISTANCE;
  temperatureKelvin = log(temperatureKelvin);
  temperatureKelvin /= Config::B_COEFFICIENT;
  temperatureKelvin += 1.0f / (Config::NOMINAL_TEMPERATURE + 273.15f);
  temperatureKelvin = 1.0f / temperatureKelvin;

  return temperatureKelvin - 273.15f;
}

}

void capteursInitialiser() {
  pinMode(Config::LDR_PIN, INPUT);
  pinMode(Config::THERMISTOR_PIN, INPUT);

  analogReadResolution(12);
  analogSetPinAttenuation(Config::LDR_PIN, ADC_11db);
  analogSetPinAttenuation(Config::THERMISTOR_PIN, ADC_11db);
}

void capteursLire() {
  mesures.lumiere = lireMoyenne(Config::LDR_PIN);
  mesures.thermistanceAdc = lireMoyenne(Config::THERMISTOR_PIN);
  mesures.temperature = convertirTemperature(mesures.thermistanceAdc);

  Serial.print("Lumiere : ");
  Serial.print(mesures.lumiere);
  Serial.print(" | Thermistance ADC : ");
  Serial.print(mesures.thermistanceAdc);
  Serial.print(" | Temperature : ");

  if (isnan(mesures.temperature)) {
    Serial.println("Erreur");
  } else {
    Serial.print(mesures.temperature, 1);
    Serial.println(" C");
  }
}

const Mesures& capteursObtenir() {
  return mesures;
}

String capteursNiveauLumiere() {
  if (mesures.lumiere < 1200) return "sombre";
  if (mesures.lumiere < 2800) return "moyen";
  return "lumineux";
}
