# Projet d'HAI912I

## Mise en route

1. Remplacer `secrets.h.example` par les valeurs réelles du wifi local
2. Renommer le fichier `secrets.h`.
3. Ouvrir `TTGO_Projet_Final_REST.ino` dans Arduino IDE
4. Selectionner `ESP32 Dev Module` et le port COM de la carte
5. Televerser et ouvrir le moniteur série a 115200 bauds

## Brochage

| Fonction | GPIO |
|---|---:|
| Photoresistance | 32 |
| Thermistance | 33 |
| LED | 25 |
| Bouton suivant | 35 |
| Bouton precedent | 0 |
| Retroeclairage | 4 |

## API

La route `GET /` affiche une interface web servie directement par l'ESP32.
Elle utilise les routes REST ci-dessous pour afficher les mesures, commander
la LED et modifier les seuils.

Routes GET :

- `GET /`
- `GET /api/v1/info`
- `GET /api/v1/sensors`
- `GET /api/v1/sensors/temperature`
- `GET /api/v1/sensors/light`
- `GET /api/v1/led`
- `GET /api/v1/threshold`

Routes PUT :

- `PUT /api/v1/led?state=on`
- `PUT /api/v1/led?state=off`
- `PUT /api/v1/led?state=toggle`
- `PUT /api/v1/led?state=auto`
- `PUT /api/v1/threshold?on=1500&off=1900`

La page web est définie dans `WebApp.cpp/.h` et est stockée en mémoire flash
avec `PROGMEM`.

Exemple :

```powershell
Invoke-RestMethod `
  -Method Put `
  -Uri "http://ADRESSE_IP/api/v1/led?state=toggle"
```
