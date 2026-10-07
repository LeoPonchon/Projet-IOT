#include "WebApp.h"

#include <Arduino.h>

namespace {

const char PAGE_WEB[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>TTGO - Station de mesure</title>
  <style>
    * { box-sizing: border-box; }
    body {
      margin: 0;
      background: #eef1f5;
      color: #20252b;
      font-family: Arial, sans-serif;
    }
    main {
      width: min(900px, 100%);
      margin: auto;
      padding: 20px;
    }
    h1 { margin: 0 0 20px; text-align: center; }
    .grille {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(230px, 1fr));
      gap: 15px;
    }
    .carte {
      padding: 18px;
      border-radius: 10px;
      background: white;
      box-shadow: 0 2px 8px #00000018;
    }
    .valeur { font-size: 2rem; margin: 12px 0; }
    button {
      margin: 4px 2px;
      padding: 9px 13px;
      border: 0;
      border-radius: 6px;
      color: white;
      cursor: pointer;
    }
    .vert { background: #2e8b57; }
    .rouge { background: #c94c4c; }
    .bleu { background: #397bbd; }
    input { width: 90px; padding: 7px; }
    #voyant {
      display: inline-block;
      width: 14px;
      height: 14px;
      margin-right: 6px;
      border-radius: 50%;
      background: #777;
    }
    .bas-page {
      margin-top: 18px;
      color: #606870;
      font-size: .9rem;
      text-align: center;
    }
    .message { min-height: 20px; }
    .routes { grid-column: 1 / -1; }
    .routes ul { margin: 0; padding-left: 20px; }
    .routes li { margin: 7px 0; }
    code { color: #245b8f; }
  </style>
</head>
<body>
  <main>
    <h1>TTGO T-Display</h1>

    <section class="grille">
      <article class="carte">
        <h2>Température</h2>
        <div class="valeur"><span id="temperature">--</span> °C</div>
      </article>

      <article class="carte">
        <h2>Luminosité</h2>
        <div class="valeur" id="luminosite">--</div>
        <div id="niveau-lumiere">--</div>
      </article>

      <article class="carte">
        <h2>LED</h2>
        <p><span id="voyant"></span><span id="etat-led">--</span></p>
        <p>Mode : <strong id="mode-led">--</strong></p>
        <button class="vert" onclick="commanderLed('on')">ON</button>
        <button class="rouge" onclick="commanderLed('off')">OFF</button>
        <button class="bleu" onclick="commanderLed('toggle')">TOGGLE</button>
        <button class="bleu" onclick="commanderLed('auto')">AUTO</button>
      </article>

      <article class="carte">
        <h2>Seuils automatiques</h2>
        <p>Allumage : <input id="seuil-on" type="number" min="0" max="4095"></p>
        <p>Extinction : <input id="seuil-off" type="number" min="0" max="4095"></p>
        <button class="bleu" onclick="enregistrerSeuils()">Enregistrer</button>
        <p class="message" id="message-seuils"></p>
      </article>

      <article class="carte routes">
        <h2>Routes API</h2>
        <ul>
          <li><code>GET /api/v1/info</code> — informations système</li>
          <li><code>GET /api/v1/sensors</code> — mesures des capteurs</li>
          <li><code>GET /api/v1/sensors/temperature</code> — température</li>
          <li><code>GET /api/v1/sensors/light</code> — luminosité</li>
          <li><code>GET /api/v1/led</code> — état de la LED</li>
          <li><code>PUT /api/v1/led?state=on|off|toggle|auto</code> — commande LED</li>
          <li><code>GET /api/v1/threshold</code> — seuils actuels</li>
          <li><code>PUT /api/v1/threshold?on=1500&amp;off=1900</code> — modification des seuils</li>
        </ul>
      </article>
    </section>

    <div class="bas-page">
      IP : <span id="ip">--</span> |
      RSSI : <span id="rssi">--</span> dBm |
      Uptime : <span id="uptime">--</span> s
    </div>
  </main>

  <script>
    async function lireCapteurs() {
      const reponse = await fetch('/api/v1/sensors');
      const donnees = await reponse.json();
      document.getElementById('temperature').textContent =
        donnees.temperature_c === null ? 'Erreur' : donnees.temperature_c;
      document.getElementById('luminosite').textContent = donnees.light_adc;
      document.getElementById('niveau-lumiere').textContent = donnees.light_level;
    }

    async function lireLed() {
      const reponse = await fetch('/api/v1/led');
      const donnees = await reponse.json();
      document.getElementById('etat-led').textContent = donnees.state.toUpperCase();
      document.getElementById('mode-led').textContent = donnees.mode.toUpperCase();
      document.getElementById('voyant').style.background =
        donnees.state === 'on' ? '#2e8b57' : '#777';

      if (document.activeElement.id !== 'seuil-on') {
        document.getElementById('seuil-on').value = donnees.threshold_on;
      }
      if (document.activeElement.id !== 'seuil-off') {
        document.getElementById('seuil-off').value = donnees.threshold_off;
      }
    }

    async function lireInfos() {
      const reponse = await fetch('/api/v1/info');
      const donnees = await reponse.json();
      document.getElementById('ip').textContent = donnees.ip;
      document.getElementById('rssi').textContent = donnees.rssi;
      document.getElementById('uptime').textContent = donnees.uptime_s;
    }

    async function actualiser() {
      try {
        await Promise.all([lireCapteurs(), lireLed(), lireInfos()]);
      } catch (erreur) {
        console.log('Erreur de lecture', erreur);
      }
    }

    async function commanderLed(etat) {
      try {
        const reponse = await fetch('/api/v1/led?state=' + etat, { method: 'PUT' });
        if (!reponse.ok) throw new Error('Commande LED refusee');
        await lireLed();
      } catch (erreur) {
        alert('Commande LED impossible');
      }
    }

    async function enregistrerSeuils() {
      const seuilOn = document.getElementById('seuil-on').value;
      const seuilOff = document.getElementById('seuil-off').value;
      const message = document.getElementById('message-seuils');

      try {
        const url = '/api/v1/threshold?on=' + encodeURIComponent(seuilOn) +
          '&off=' + encodeURIComponent(seuilOff);
        const reponse = await fetch(url, { method: 'PUT' });
        const donnees = await reponse.json();
        message.textContent = reponse.ok ? 'Seuils enregistrés' : donnees.error;
      } catch (erreur) {
        message.textContent = 'Erreur de communication';
      }
    }

    actualiser();
    setInterval(actualiser, 2000);
  </script>
</body>
</html>
)rawliteral";

}

const char* webAppObtenir() {
  return PAGE_WEB;
}
