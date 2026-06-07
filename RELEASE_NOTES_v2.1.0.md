# Release v2.1.0

## Highlights

- Steuerung von zwei Garagentoren mit ESP32 ueber lokale Weboberflaeche
- Ansteuerung von zwei Relais fuer potentialfreien Tastereingang (z. B. Hoermann)
- Dokumentierte WLAN-Reconnect-Logik bei Verbindungsabriss und schwachem Signal
- Projektdokumentation und Lizenzierung fuer die oeffentliche Bereitstellung

## Inhalt dieser Version

- README mit Hardwarebeschreibung, Sicherheitshinweisen und Inbetriebnahme
- MIT-Lizenz hinzugefuegt
- Third-Party-Hinweis zum Referenzprojekt hinzugefuegt
- Changelog eingefuehrt
- GitHub-Tag v2.1.0 erstellt

## Hinweise

- WLAN-Zugangsdaten bleiben bewusst als Platzhalter im Sketch und muessen lokal angepasst werden.
- Das referenzierte Projekt WebServer-esp32 (LGPL-2.1) ist als Referenz dokumentiert.
  Im aktuellen Sketch wird keine direkte WebServer-API dieses Projekts eingebunden.
