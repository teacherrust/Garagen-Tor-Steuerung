# ESP32 Garagentorsteuerung (2 Tore)

Dieses Projekt steuert zwei Garagentore mit einem ESP32 per Weboberflaeche.

Der ESP32 schaltet zwei Relais, die jeweils den potentialfreien Tastereingang einer Torsteuerung kurzschliessen (z. B. Hoermann). Das Verhalten entspricht einem kurzen Tastimpuls am Wandtaster.

## Wichtiger Sicherheitshinweis

Dieses Projekt steuert sicherheitsrelevante Hardware. Vor produktivem Einsatz muessen alle Sicherheitsfunktionen der Toranlage (Lichtschranke, Hinderniserkennung, Endlagen, Not-Aus) korrekt funktionieren und geprueft werden. Nutzung auf eigenes Risiko.

## Funktionen

- Steuerung von linkem und rechtem Tor ueber lokale Weboberflaeche
- Statusanzeige (offen/geschlossen) je Tor per Sensor-Eingang
- Automatisches Schliessen nach konfigurierbarer Zeit (5 min, 30 min, 60 min, 12 h)
- WLAN-Reconnect-Logik bei Verbindungsabriss
- Reconnect auch bei dauerhaft schwachem RSSI-Signal

## Hardware

- 1x ESP32
- 2x Relaiskanaele (fuer potentialfreien Schaltkontakt)
- 2x Sensor-Eingaenge (z. B. Lichtschranke / Endlagenabfrage)
- Garagentorantrieb mit Tastereingang (z. B. Hoermann)

## Verdrahtung (im Sketch hinterlegt)

- Relais links: GPIO 26
- Relais rechts: GPIO 27
- Sensor links: GPIO 34
- Sensor rechts: GPIO 35

Hinweis: GPIO 34/35 sind beim ESP32 nur als Eingang nutzbar.

## WLAN-Konfiguration

Die WLAN-Daten im Sketch sind absichtlich Platzhalter und muessen vor dem Einsatz angepasst werden.

Datei: ESP32_Garagentor_21_Verbesserungen_weitergabe.ino

```cpp
const char* ssid = "test";
const char* password = "12345678";
```

## WLAN-Reconnect bei Verbindungsabriss

Die Reconnect-Logik ist bereits im Code enthalten:

- Zyklische Pruefung im Intervall `WIFI_RECONNECT_INTERVAL` (30 s)
- Bei `WiFi.status() != WL_CONNECTED`: Trennen und neu verbinden
- Bei zu schwachem Signal (`WiFi.RSSI() < WIFI_SIGNAL_THRESHOLD`): Trennen und neu verbinden
- Falls die Verbindung beim Start scheitert, startet der ESP neu

## Weboberflaeche

Nach erfolgreicher WLAN-Verbindung zeigt der Serial Monitor die lokale IP-Adresse an. Diese IP im Browser aufrufen, um die Taster und Statusanzeige zu nutzen.

## Build/Upload

- Arduino IDE oder PlatformIO mit ESP32-Boardpaket
- Sketch kompilieren und auf ESP32 laden
- Seriellen Monitor mit 115200 Baud oeffnen

## Lizenz

Dieses Projekt steht unter der MIT-Lizenz. Siehe LICENSE.

## Drittanbieter und Lizenzrelevanz

Als externe Referenz wurde genannt:
- https://github.com/teacherrust/WebServer-esp32 (LGPL-2.1)

Einschaetzung fuer dieses Repository:
- Im aktuellen Sketch wird keine `WebServer`-API aus diesem Projekt eingebunden.
- Es werden `WiFi.h` und `WiFiServer` genutzt.
- Damit ist das genannte Repository nach aktuellem Stand eher Inspirationsquelle als direkte Codeabhaengigkeit.

Wenn spaeter Code aus dem LGPL-Projekt uebernommen oder dessen Library direkt gelinkt wird, muessen die LGPL-Bedingungen zusaetzlich eingehalten werden.
