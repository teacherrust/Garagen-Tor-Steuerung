/*********
  Garagentor - Automatik + Handy

  !!!! WiFi- Name und Passwort ändern!!!!

  (cc) Volker Rust
  (v)  1.8      09.08.2022
  (v)  2.1      29.09.2025

  in use:
  1x ESP32: mit Webserver !!! W-Lan-Anbindung nötig, oder W-Lan mit ESP aufspannen.
  2x E18-D80NK: Infrarot-Lichtschranke (Funktioniert wie Taster - 5V über Vin-PIN)
  1x Doppelrelais: An potentiallosen Schalter-Eingang des Torantriebs.

  letzte Änderungen
  2.1
  - Tore-Klasse - Tore als Objekte
  - Weiniger Strings wegen der Speicherverwaltung
  - Mehr Fehlerbehebungsfunktionen - im WiFi etc. 
  1.8
  - WiFi-Reconnect richtig
  - offenZeit 60 Minuten Bug gesucht
  1.7
  - WiFi.setAutoReconnect(true);
  - Farbe der Taster angepasst.
*********/


#include <WiFi.h>

// Konstanten
const unsigned long OPEN_TIME_DEFAULT = 30;  // Minuten
const unsigned long WIFI_RECONNECT_INTERVAL = 30000;  // 30 Sekunden
const unsigned long CLIENT_TIMEOUT = 5000;  // 5 Sekunden
const unsigned long DOOR_IMPULSE_DURATION = 800;  // Millisekunden
const unsigned long DOOR_CHECK_INTERVAL = 1000;  // 1 Sekunde
const int MAX_DOOR_CHECK_ATTEMPTS = 14;
const int WIFI_SIGNAL_THRESHOLD = -75;  // dBm

// WiFi-Konfiguration
const char* ssid = "test";
const char* password = "12345678";
// Pin-Konfiguration
const int PIN_TOR_LINKS = 26;
const int PIN_TOR_RECHTS = 27;
const int PIN_MESS_LINKS = 34;
const int PIN_MESS_RECHTS = 35;

// Farben
const char* COLOR_GREEN = "#44AA55";
const char* COLOR_RED = "#FF1100";
const char* COLOR_ORANGE = "#FFA500";

// Klasse für das Tor
class Tor {
private:
    int pin;  // Pin-Nummer für das Tor
    int messPin;  // Pin-Nummer für den Messsensor
    bool closed;  // Status des Tores
    unsigned long openTime;  // Zeit, zu der das Tor geöffnet wurde
    unsigned long lastSwitched;  // Zeit, zu der das Tor zuletzt geschaltet wurde

public:
    Tor(int pinNumber, int messPinNumber) 
        : pin(pinNumber), messPin(messPinNumber), closed(true), openTime(0), lastSwitched(0) {
        pinMode(pin, OUTPUT);
        pinMode(messPin, INPUT);
        digitalWrite(pin, LOW);
    }

    void toggle() {
        digitalWrite(pin, HIGH);
        delay(DOOR_IMPULSE_DURATION);
        digitalWrite(pin, LOW);
        lastSwitched = millis();
        closed = !closed;  // Status umkehren
        if (!closed) {
            openTime = millis();  // Tor geöffnet
        } else {
            openTime = 0;  // Tor geschlossen
        }
    }

    void checkStatus() {
        closed = digitalRead(messPin);
        if (!closed && openTime == 0) {
            openTime = millis();
        } else if (closed) {
            openTime = 0;
        }
    }

    bool isClosed() const {
        return closed;
    }

    unsigned long getOpenDuration() const {
        if (!closed) {
            return millis() - openTime;
        }
        return 0;
    }

    unsigned long getLastSwitchedTime() const {
        return lastSwitched;
    }
};

// Globale Variablen
WiFiServer server(80);
Tor torLinks(PIN_TOR_LINKS, PIN_MESS_LINKS);
Tor torRechts(PIN_TOR_RECHTS, PIN_MESS_RECHTS);
unsigned long openTime = OPEN_TIME_DEFAULT;
unsigned long previousMillis = 0;

void setup() {
    Serial.begin(115200);
    initWiFi();
    server.begin();
}

void loop() {
    checkAndReconnectWiFi();
    torLinks.checkStatus();
    torRechts.checkStatus();
    handleWebServer();
    automaticDoorClose();
}

void initWiFi() {
    Serial.print("Verbinde mit WiFi ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Verbindung fehlgeschlagen. Neustart in 5 Sekunden.");
        delay(5000);
        ESP.restart();
    }

    Serial.println("");
    Serial.println("WiFi verbunden.");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
}

void checkAndReconnectWiFi() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= WIFI_RECONNECT_INTERVAL) {
        previousMillis = currentMillis;

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi-Verbindung verloren. Neuverbindung...");
            WiFi.disconnect();
            initWiFi();
        } else if (WiFi.RSSI() < WIFI_SIGNAL_THRESHOLD) {
            Serial.println("Signal schwach. Neuverbindung...");
            WiFi.disconnect();
            initWiFi();
        }
    }
}

void handleWebServer() {
    WiFiClient client = server.available();
    if (client) {
        unsigned long clientConnectTime = millis();
        String request = client.readStringUntil('\r');
        client.flush();

        bool redirect = processClientRequest(client, request.c_str());

        if (redirect) {
            client.println("HTTP/1.1 303 See Other");
            client.println("Location: /");
            client.println("Connection: close");
            client.println();
        } else {
            sendHtmlResponse(client);
        }

        client.stop();
    }
}

bool processClientRequest(WiFiClient& client, const char* header) {
    bool actionPerformed = false;

    if (strstr(header, "GET /torLinks") != NULL) {
        torLinks.toggle();
        actionPerformed = true;
    } else if (strstr(header, "GET /torRechts") != NULL) {
        torRechts.toggle();
        actionPerformed = true;
    } else if (strstr(header, "GET /5min") != NULL) {
        openTime = 5;
        actionPerformed = true;
    } else if (strstr(header, "GET /30min") != NULL) {
        openTime = 30;
        actionPerformed = true;
    } else if (strstr(header, "GET /60min") != NULL) {
        openTime = 60;
        actionPerformed = true;
    } else if (strstr(header, "GET /12h") != NULL) {
        openTime = 720;
        actionPerformed = true;
    }

    return actionPerformed;
}

void sendHtmlResponse(WiFiClient& client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    
    const char* leftButtonColor = (millis() - torLinks.getLastSwitchedTime() < 12000) ? COLOR_ORANGE : 
                                  (torLinks.isClosed() ? COLOR_GREEN : COLOR_RED);
    const char* rightButtonColor = (millis() - torRechts.getLastSwitchedTime() < 12000) ? COLOR_ORANGE : 
                                   (torRechts.isClosed() ? COLOR_GREEN : COLOR_RED);
    
    client.println(".buttonL, .buttonR, .buttonCheck, .button2 { border-radius: 5px; box-shadow: 4px 4px 5px rgba(0,0,0,0.3); border: 1px solid geay; color: white; padding: 22px 40px; text-decoration: none; font-size: 20px; margin: 10px; cursor: pointer;}");
    client.println(".buttonL { background-color: " + String(leftButtonColor) + "; }");
    client.println(".buttonR { background-color: " + String(rightButtonColor) + "; }");
    client.println(".buttonCheck { background-color: #55BBCC; padding: 8px 40px; }");
    client.println(".button2 { background-color: #C9E1C8; padding: 8px 20px; font-size: 18px; }");
    client.println("</style></head>");

    client.println("<body><h1>Garagen-TOR-Server</h1>");
    client.println("<p><a href=\"/torLinks\"><button class=\"buttonL\">L</button></a> &nbsp;&nbsp;&nbsp; <a href=\"/torRechts\"><button class=\"buttonR\">R</button></a></p>");

    if (!torLinks.isClosed() && !torRechts.isClosed()) {
        client.println("<p><font color=FF0000>Beide Tore sind offen. </font></p>");
    } else if (!torRechts.isClosed()) {
        client.println("<p><font color=red>Rechtes Tor ist offen. </font></p>");
    } else if (!torLinks.isClosed()) {
        client.println("<p><font color=red>Linkes Tor ist offen. </font></p>");
    } else {
        client.println("<p>Beide Tore sind geschlossen. </p>");
    }
    
    client.println("<p><a href=\"/\"><button class=\"buttonCheck\">Tor-Check</button></a> </p>");
    client.println("<p>automatisch zu in: ");
    
    if (!torRechts.isClosed() || !torLinks.isClosed()) {
        unsigned long openDuration = millis() - (torLinks.isClosed() ? torRechts.getOpenDuration() : torLinks.getOpenDuration());
        int restZeit = openTime - (openDuration / 60000);
        restZeit = restZeit > 0 ? restZeit : 0;  // Ensure non-negative value
        client.println( String(restZeit) + " min</p>");
    } else {
        client.println(" --- </p>");
    }

    client.println("<p>Stelle Standard-Auto-Close-Time: </p>");
    client.println("<p><a href=\"/5min\"><button class=\"button2\">5 min</button></a> <a href=\"/30min\"><button class=\"button2\">30 min</button></a> <a href=\"/60min\"><button class=\"button2\">60 min</button></a> <a href=\"/12h\"><button class=\"button2\">12 h</button></a></p>");
    client.println("<p></p>");
    client.println("<p>aktuelle Auto-Close-Time: ");

    char openTimeStr[16];
    if (openTime < 720) {
        snprintf(openTimeStr, sizeof(openTimeStr), "%lu min", openTime);
    } else {
        snprintf(openTimeStr, sizeof(openTimeStr), "%lu h", openTime / 60);
    }
    client.println(String(openTimeStr) + "</p>");

    char rssiStr[32];
    snprintf(rssiStr, sizeof(rssiStr), "WiFi-RSSI: %d dB", WiFi.RSSI());
    client.println(rssiStr);

    client.println("</body></html>");
}

void automaticDoorClose() {
    unsigned long currentMillis = millis();
    
    Tor* tores[] = { &torLinks, &torRechts };  // Array von Zeigern auf die Tore

    for (int i = 0; i < 2; i++) {
        Tor* currentTor = tores[i];  // Aktuelles Tor

        if (!currentTor->isClosed()) {
            unsigned long openDuration = currentTor->getOpenDuration();
            if (openDuration > openTime * 60000) {
                // Tor einmal auslösen, um es zu schließen
                currentTor->toggle();  
                delay(1000);  // Warte kurz, um sicherzustellen, dass das Tor reagiert
                for (int attempts = 0; attempts < MAX_DOOR_CHECK_ATTEMPTS; attempts++) {
                    delay(1000);  // Warte eine Sekunde
                    currentTor->checkStatus();  // Überprüfe den Status des Tores
                    if (currentTor->isClosed()) {
                        break;  // Beende die Schleife, wenn das Tor geschlossen ist
                    }
                }
                // Wenn das Tor nach 14 Sekunden immer noch offen ist, schließe es erneut
                if (!currentTor->isClosed()) {
                    currentTor->toggle();
                }
            }
        }
    }
}
