 #ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>  
#include <ArduinoJson.h>
#include <DHT.h>

// Replace with your network credentials
const char* ssid = "your_ssid";
const char* password = "your_password";

// Initialize Telegram BOT
#define BOTtoken "replace Your bot Token from botfather"  

#define CHAT_ID "replace id From IdBot"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin1 = D0;
const int ledPin2 = D4;

bool ledState1 = LOW;
bool ledState2 = LOW;

#define DHTPIN D8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Handle received messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands:.\n\n";

      welcome += "/ledOne_on to turn  ON LED 1 \n";
      welcome += "/ledOne_off to turn  OFF LED 1 \n";

      welcome += "/ledTwo_on to turn ON  LED 2 \n";
      welcome += "/ledTwo_off to turn OFF LED 2 \n";

      welcome += "/dht to get Temp & Humidity data \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    //Led 1 
    if (text == "/ledOne_on") {
      bot.sendMessage(chat_id, "LEDone ON", "");
      ledState1 = HIGH;
      digitalWrite(ledPin1, ledState1);
    }
    
    if (text == "/ledOne_off") {
      bot.sendMessage(chat_id, "LEDone OFF", "");
      ledState1 = LOW;
      digitalWrite(ledPin1, ledState1);
    }

    //led 2
    if (text == "/ledTwo_on") {
      bot.sendMessage(chat_id, "LEDTwo ON", "");
      ledState2 = HIGH;
      digitalWrite(ledPin2, ledState2);
    }
    
    if (text == "/ledTwo_off") {
      bot.sendMessage(chat_id, "LEDTwo OFF", ""); 
      ledState2 = LOW;
      digitalWrite(ledPin2, ledState2);
    }
    
    if (text == "/dht") {
      String DHTdata = getDHT();
      bot.sendMessage(chat_id, DHTdata, "");
    }
  }
}
String getDHT() {
  float temperature, humidity;
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  String message = "Temperature: " + String(temperature) + " ºC \n";
  message += "Humidity: " + String(humidity) + " % \n";
  return message;
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
   configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert);         // Add root certificate for api.telegram.org
  #endif

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, ledState1);
  digitalWrite(ledPin2, ledState2);

   dht.begin();

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);   // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}