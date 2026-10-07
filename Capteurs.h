#pragma once

#include <Arduino.h>

struct Mesures {
  int lumiere;
  int thermistanceAdc;
  float temperature;
};

void capteursInitialiser();
void capteursLire();
const Mesures& capteursObtenir();
String capteursNiveauLumiere();
