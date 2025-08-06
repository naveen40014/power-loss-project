/*
  Power-Loss SMS Alert System
  ESP32 + SIM800A  |  Stores up to 4 phone numbers via on-board Wi-Fi page
  Access-Point SSID : PowerMonitor   |  Password : 12345678
  Web UI URL        : http://192.168.4.1

  • Relay OUT goes LOW when the printer’s 5 V rail disappears.
  • ESP32 keeps running from the battery for a few seconds.
  • It sends an SMS to every stored number through the SIM800A.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

// ---------- hardware mapping ----------
#define RELAY_PIN 14          // HIGH = power OK, LOW = power lost
#define SIM_TX    17          // ESP32 TX2  -> SIM800A RX
#define SIM_RX    16          // ESP32 RX2  -> SIM800A TX
#define SIM_BAUD  9600        // try 115200 if 9600 prints gibberish
// --------------------------------------

// ---------- Wi-Fi AP credentials -------
const char* AP_SSID = "PowerMonitor";
const char* AP_PASS = "12345678";
// --------------------------------------

HardwareSerial sim(2);        // use Serial2 (UART2)
WebServer      server(80);    // tiny web server on port 80
Preferences    prefs;         // flash key-value storage

String numbers[4];            // up to 4 phone numbers (E.164, e.g. +919876543210)

// ---------- helpers ----------
void loadNumbers() {
  prefs.begin("nums", true);               // read-only
  for (int i = 0; i < 4; i++)
    numbers[i] = prefs.getString(("n" + String(i)).c_str(), "");
  prefs.end();
}

void saveNumbers() {
  prefs.begin("nums", false);              // write mode
  for (int i = 0; i < 4; i++)
    prefs.putString(("n" + String(i)).c_str(), numbers[i]);
  prefs.end();
}

void sendAT(const String& cmd, uint16_t wait = 500) {
  sim.println(cmd);
  delay(wait);
}

void sendSMS(const String& phone, const String& msg) {
  if (phone.length() < 6) return;          // ignore empty slot
  sendAT("AT+CMGF=1");                     // text mode
  sendAT("AT+CMGS=\"" + phone + "\"");
  sim.print(msg);
  sim.write(26);                           // Ctrl-Z terminator
  delay(5000);                             // wait for send
}

// ---------- Web interface ----------
void handleRoot() {
  String page =
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<title>Power Monitor</title>"
    "<style>body{font-family:sans-serif;margin:40px;}input{width:210px;}</style>"
    "</head><body><h2>Set Alert Numbers</h2><form method='POST' action='/save'>";
  for (int i = 0; i < 4; i++) {
    page += "Number " + String(i + 1) + ": <input name='n" + String(i) +
            "' value='" + numbers[i] + "'><br><br>";
  }
  page += "<input type='submit' value='Save'></form></body></html>";
  server.send(200, "text/html", page);
}

void handleSave() {
  for (int i = 0; i < 4; i++) {
    String f = "n" + String(i);
    if (server.hasArg(f)) {
      String num = server.arg(f);
      num.trim();
      numbers[i] = num;
    }
  }
  saveNumbers();
  server.sendHeader("Location", "/");
  server.send(303);                         // redirect (POST-redirect-GET)
}

void setupWeb() {
  WiFi.softAP(AP_SSID, AP_PASS);
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  Serial.print("AP up - connect to ");
  Serial.println(AP_SSID);
}

// ---------- setup ----------
void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, INPUT);

  sim.begin(SIM_BAUD, SERIAL_8N1, SIM_RX, SIM_TX);
  loadNumbers();
  setupWeb();

  Serial.println("System ready.");
}

// ---------- loop ----------
bool smsSent = false;

void loop() {
  server.handleClient();

  int pwr = digitalRead(RELAY_PIN);       // HIGH = mains OK

  if (pwr == LOW && !smsSent) {
    String alert = "⚠ Power lost on 3D printer!";
    for (auto &num : numbers) sendSMS(num, alert);
    smsSent = true;
  }
  if (pwr == HIGH) smsSent = false;       // reset after mains restore
}
