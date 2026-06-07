# Changelog

Alle nennenswerten Aenderungen an diesem Projekt werden in dieser Datei dokumentiert.

## [2.3.0] - 2026-06-07

### Changed
- Relais-Impuls im Tor-Toggle auf non-blocking Ablauf umgestellt
- Auto-Close um Serial-Diagnosemeldungen (Impuls, Retry, Erfolg, Fallback) erweitert

## [2.2.0] - 2026-06-07

### Changed
- Auto-Close auf non-blocking Zustandslogik umgestellt (keine langen `delay`-Schleifen)
- Bessere Inline-Kommentare an den kritischen Ablaufstellen
- Versionsstand im Sketch auf 2.2 angehoben

## [2.1.0] - 2026-06-07

### Added
- README mit Hardwarebeschreibung, Sicherheits- und Setup-Hinweisen
- MIT-Lizenzdatei hinzugefuegt
- Third-Party-Hinweis zur externen Referenz erstellt
- .gitignore fuer typische Arduino-Artefakte hinzugefuegt

### Changed
- Projekt als Git-Repository strukturiert und auf GitHub veroeffentlicht
- Dokumentation zur WLAN-Reconnect-Logik ergaenzt

### Notes
- WLAN-Zugangsdaten bleiben bewusst als Platzhalter im Sketch und werden in der Doku erklaert.
- Externes Referenzprojekt WebServer-esp32 ist dokumentiert; im aktuellen Sketch keine direkte API-Nutzung dieser Library.
