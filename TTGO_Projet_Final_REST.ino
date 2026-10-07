#include <Arduino.h>
#include <WiFi.h>

#include "secrets.h"
#include "Capteurs.h"
#include "Led.h"
#include "Affichage.h"
#include "Api.h"

unsigned long derniereLecture = 0;
unsigned long derniereTentativeWifi = 0;

void connecterWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ttgo-capteurs");
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connexion Wi-Fi");
  unsigned long debut = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - debut < 20000) {
    delay(500);
    Serial.print('.');
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connecte");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
    apiDemarrer();
  } else {
    Serial.println("Connexion Wi-Fi impossible");
  }

  derniereTentativeWifi = millis();
}

void surveillerWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    apiDemarrerSiNecessaire();
    apiTraiter();
    return;
  }

  if (millis() - derniereTentativeWifi >= 10000) {
    Serial.println("Nouvelle tentative Wi-Fi");
    WiFi.reconnect();
    derniereTentativeWifi = millis();
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  capteursInitialiser();
  ledInitialiser();
  affichageInitialiser();
  apiConfigurer();

  capteursLire();
  ledActualiser(capteursObtenir().lumiere);
  affichageActualiser();

  connecterWifi();
}

void loop() {
  affichageLireBoutons();
  surveillerWifi();

  if (millis() - derniereLecture >= 1000) {
    capteursLire();
    ledActualiser(capteursObtenir().lumiere);
    affichageActualiser();
    derniereLecture = millis();
  }

  delay(10);
}
