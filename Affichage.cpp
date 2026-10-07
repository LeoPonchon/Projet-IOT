#include "Affichage.h"

#include <TFT_eSPI.h>
#include <math.h>

#include "Capteurs.h"
#include "Config.h"
#include "Led.h"

namespace {

TFT_eSPI ecran;
int page = 0;
bool ancienEtatSuivant = HIGH;
bool ancienEtatPrecedent = HIGH;
unsigned long dernierBouton = 0;

}

void affichageInitialiser() {
  pinMode(Config::BUTTON_NEXT, INPUT);
  pinMode(Config::BUTTON_PREVIOUS, INPUT_PULLUP);
  pinMode(Config::BACKLIGHT_PIN, OUTPUT);

  digitalWrite(Config::BACKLIGHT_PIN, HIGH);

  ecran.init();
  ecran.setRotation(1);
  ecran.setTextSize(2);
}

void affichageActualiser() {
  const Mesures& mesures = capteursObtenir();

  ecran.fillScreen(TFT_BLACK);
  ecran.setTextSize(2);
  ecran.setTextColor(TFT_GREEN, TFT_BLACK);

  if (page == 0) {
    ecran.drawString("TEMPERATURE", 10, 20);
    ecran.setTextColor(TFT_WHITE, TFT_BLACK);

    if (isnan(mesures.temperature)) {
      ecran.drawString("Erreur lecture", 10, 60);
    } else {
      ecran.drawString(String(mesures.temperature, 1) + " C", 10, 60);
    }

    ecran.setTextColor(TFT_YELLOW, TFT_BLACK);
    ecran.drawString("Page 1/3", 10, 110);
    return;
  }

  if (page == 1) {
    ecran.drawString("PHOTORESISTANCE", 10, 20);
    ecran.setTextColor(TFT_WHITE, TFT_BLACK);
    ecran.drawString("Valeur : " + String(mesures.lumiere), 10, 60);
    ecran.drawString(capteursNiveauLumiere(), 10, 90);
    ecran.setTextColor(TFT_YELLOW, TFT_BLACK);
    ecran.drawString("Page 2/3", 10, 115);
    return;
  }

  ecran.drawString("LED", 10, 20);
  ecran.setTextColor(TFT_WHITE, TFT_BLACK);
  ecran.drawString(ledEstAllumee() ? "Etat : ON" : "Etat : OFF", 10, 55);
  ecran.drawString(
    ledEstAutomatique() ? "Mode : AUTO" : "Mode : MANUEL",
    10,
    85
  );
  ecran.setTextColor(TFT_YELLOW, TFT_BLACK);
  ecran.drawString("Page 3/3", 10, 115);
}

void affichageLireBoutons() {
  bool etatSuivant = digitalRead(Config::BUTTON_NEXT);
  bool etatPrecedent = digitalRead(Config::BUTTON_PREVIOUS);

  if (millis() - dernierBouton >= 250) {
    if (ancienEtatSuivant == HIGH && etatSuivant == LOW) {
      page = (page + 1) % 3;
      affichageActualiser();
      dernierBouton = millis();
    }

    if (ancienEtatPrecedent == HIGH && etatPrecedent == LOW) {
      page = (page + 2) % 3;
      affichageActualiser();
      dernierBouton = millis();
    }
  }

  ancienEtatSuivant = etatSuivant;
  ancienEtatPrecedent = etatPrecedent;
}
