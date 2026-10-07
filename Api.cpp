#include "Api.h"

#include <WiFi.h>
#include <WebServer.h>
#include <math.h>

#include "Affichage.h"
#include "Capteurs.h"
#include "Led.h"
#include "WebApp.h"

namespace {

WebServer serveur(80);
bool serveurDemarre = false;

void envoyerJson(int code, const String& contenu) {
  serveur.send(code, "application/json", contenu);
}

void routeAccueil() {
  serveur.send_P(200, "text/html; charset=utf-8", webAppObtenir());
}

void routeInfo() {
  String json = "{";
  json += "\"device\":\"TTGO T-Display\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  json += "\"uptime_s\":" + String(millis() / 1000);
  json += "}";
  envoyerJson(200, json);
}

void routeSensors() {
  const Mesures& mesures = capteursObtenir();
  String json = "{";
  json += "\"sensors\":[\"temperature\",\"light\"],";
  json += "\"light_adc\":" + String(mesures.lumiere) + ",";
  json += "\"light_level\":\"" + capteursNiveauLumiere() + "\",";
  json += "\"thermistor_adc\":" + String(mesures.thermistanceAdc) + ",";
  json += "\"temperature_c\":";

  if (isnan(mesures.temperature)) {
    json += "null";
  } else {
    json += String(mesures.temperature, 1);
  }

  json += "}";
  envoyerJson(200, json);
}

void routeTemperature() {
  const Mesures& mesures = capteursObtenir();
  String json = "{\"temperature_c\":";

  if (isnan(mesures.temperature)) {
    json += "null";
  } else {
    json += String(mesures.temperature, 1);
  }

  json += "}";
  envoyerJson(200, json);
}

void routeLight() {
  const Mesures& mesures = capteursObtenir();
  String json = "{";
  json += "\"value_adc\":" + String(mesures.lumiere) + ",";
  json += "\"level\":\"" + capteursNiveauLumiere() + "\"}";
  envoyerJson(200, json);
}

void routeLedStatus() {
  String json = "{";
  json += "\"state\":\"" + String(ledEstAllumee() ? "on" : "off") + "\",";
  json += "\"mode\":\"" + String(ledEstAutomatique() ? "auto" : "manual") + "\",";
  json += "\"threshold_on\":" + String(ledSeuilAllumage()) + ",";
  json += "\"threshold_off\":" + String(ledSeuilExtinction());
  json += "}";
  envoyerJson(200, json);
}

void routeLedCommand() {
  if (!serveur.hasArg("state")) {
    envoyerJson(400, "{\"error\":\"state is required\"}");
    return;
  }

  String state = serveur.arg("state");
  state.toLowerCase();
  state.trim();

  if (state == "on") {
    ledModeManuel();
    ledAllumer();
  } else if (state == "off") {
    ledModeManuel();
    ledEteindre();
  } else if (state == "toggle") {
    ledModeManuel();
    ledBasculer();
  } else if (state == "auto") {
    ledActiverAutomatique();
    ledActualiser(capteursObtenir().lumiere);
  } else {
    envoyerJson(400, "{\"error\":\"invalid state\"}");
    return;
  }

  affichageActualiser();
  routeLedStatus();
}

bool lireEntier(const char* nom, int& valeur) {
  if (!serveur.hasArg(nom)) return false;

  String texte = serveur.arg(nom);
  if (texte.length() == 0) return false;

  for (unsigned int i = 0; i < texte.length(); i++) {
    if (texte[i] < '0' || texte[i] > '9') return false;
  }

  valeur = texte.toInt();
  return true;
}

void routeThresholdGet() {
  String json = "{";
  json += "\"on\":" + String(ledSeuilAllumage()) + ",";
  json += "\"off\":" + String(ledSeuilExtinction());
  json += "}";
  envoyerJson(200, json);
}

void routeThresholdPut() {
  int nouveauSeuilOn;
  int nouveauSeuilOff;

  if (!lireEntier("on", nouveauSeuilOn) ||
      !lireEntier("off", nouveauSeuilOff)) {
    envoyerJson(400, "{\"error\":\"on and off are required\"}");
    return;
  }

  if (!ledDefinirSeuils(nouveauSeuilOn, nouveauSeuilOff)) {
    envoyerJson(400, "{\"error\":\"invalid thresholds\"}");
    return;
  }

  ledActualiser(capteursObtenir().lumiere);
  routeThresholdGet();
}

void routeNotFound() {
  envoyerJson(404, "{\"error\":\"route not found\"}");
}

}

void apiConfigurer() {
  serveur.on("/", HTTP_GET, routeAccueil);
  serveur.on("/api/v1/info", HTTP_GET, routeInfo);
  serveur.on("/api/v1/sensors", HTTP_GET, routeSensors);
  serveur.on("/api/v1/sensors/temperature", HTTP_GET, routeTemperature);
  serveur.on("/api/v1/sensors/light", HTTP_GET, routeLight);
  serveur.on("/api/v1/led", HTTP_GET, routeLedStatus);
  serveur.on("/api/v1/led", HTTP_PUT, routeLedCommand);
  serveur.on("/api/v1/threshold", HTTP_GET, routeThresholdGet);
  serveur.on("/api/v1/threshold", HTTP_PUT, routeThresholdPut);
  serveur.onNotFound(routeNotFound);
}

void apiDemarrer() {
  if (!serveurDemarre) {
    serveur.begin();
    serveurDemarre = true;
    Serial.println("Serveur HTTP demarre");
  }
}

void apiDemarrerSiNecessaire() {
  apiDemarrer();
}

void apiTraiter() {
  serveur.handleClient();
}
