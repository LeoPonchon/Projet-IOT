#include "Led.h"

#include "Config.h"

namespace {

bool etatLed = false;
bool modeAutomatique = true;
int seuilAllumage = Config::DEFAULT_THRESHOLD_ON;
int seuilExtinction = Config::DEFAULT_THRESHOLD_OFF;

void appliquerEtat(bool etat) {
  etatLed = etat;
  digitalWrite(Config::LED_PIN, etat ? HIGH : LOW);
}

}

void ledInitialiser() {
  pinMode(Config::LED_PIN, OUTPUT);
  appliquerEtat(false);
}

void ledAllumer() {
  appliquerEtat(true);
}

void ledEteindre() {
  appliquerEtat(false);
}

void ledBasculer() {
  appliquerEtat(!etatLed);
}

void ledModeManuel() {
  modeAutomatique = false;
}

void ledActiverAutomatique() {
  modeAutomatique = true;
}

void ledActualiser(int valeurLumiere) {
  if (!modeAutomatique) return;

  if (!etatLed && valeurLumiere < seuilAllumage) {
    appliquerEtat(true);
  }

  if (etatLed && valeurLumiere > seuilExtinction) {
    appliquerEtat(false);
  }
}

bool ledEstAllumee() {
  return etatLed;
}

bool ledEstAutomatique() {
  return modeAutomatique;
}

int ledSeuilAllumage() {
  return seuilAllumage;
}

int ledSeuilExtinction() {
  return seuilExtinction;
}

bool ledDefinirSeuils(int allumage, int extinction) {
  if (allumage < 0 || allumage > 4095 ||
      extinction < 0 || extinction > 4095 ||
      extinction <= allumage) {
    return false;
  }

  seuilAllumage = allumage;
  seuilExtinction = extinction;
  return true;
}
