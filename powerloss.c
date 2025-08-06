#define SIM_TX 17  // ESP32 TX2
#define SIM_RX 16  // ESP32 RX2
const int relayPin = 14; // D14 connected to relay OUT

bool smsSent = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, SIM_RX, SIM_TX); // Use Serial2 for SIM800L
  pinMode(relayPin, INPUT);
  delay(1000);
  Serial.println("ESP32 + SIM800L Power Loss Alert Initialized");
}

void loop() {
  int powerState = digitalRead(relayPin);

  if (powerState == LOW && !smsSent) {
    sendSMS("⚠️ Power lost on your 3D printer!");
    smsSent = true;
  }

  if (powerState == HIGH) {
    smsSent = false; // Reset when power comes back
  }

  delay(1000);
}

void sendSMS(String msg) {
  Serial2.println("AT+CMGF=1"); // Set to text mode
  delay(500);
  Serial2.println("AT+CMGS=\"+91XXXXXXXXXX\""); // Your mobile number
  delay(500);
  Serial2.print(msg);
  Serial2.write(26); // CTRL+Z to send SMS
  delay(5000);
}
