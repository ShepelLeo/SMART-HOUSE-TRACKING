#include <DHT.h>
#include <DHT_U.h>

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
//#include <TroykaThermometer.h>
#include <TroykaMQ.h>

#define FIREBASE_HOST "fablab-moscow.firebaseio.com"
#define FIREBASE_AUTH "6qDiLs8Tpli9jUYTv4FciVz0HC38dHLEZDTuBPiq"
#define WIFI_SSID "fablab"
#define WIFI_PASSWORD "fablab77"


//#define RELAY_4 D4

#define DHTPIN D1
#define DHTTYPE DHT11

//#define LIGHTSENSOR A0

#define PIN_MQ9  A0
// имя для пина, к которому подключен нагреватель датчика
#define PIN_MQ9_HEATER  D0
 
// создаём объект для работы с датчиком
// и передаём ему номер пина выходного сигнала и нагревателя
MQ9 mq9(PIN_MQ9, PIN_MQ9_HEATER);

DHT dht(DHTPIN, DHTTYPE);

//TroykaThermometer thermometer(A0);

float light = 0;
void setup() {
  Serial.begin(9600);
  //pinMode(RELAY_4, OUTPUT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  Firebase.set("LPG", 0);
  Firebase.set("Methane", 0);
  Firebase.set("CarbonMonoxide", 0);
  //dht.begin();

  mq9.cycleHeat();
  dht.begin();
}


void loop() {

  //digitalWrite(ledPin, Firebase.getInt("redlight"));
  if (!mq9.isCalibrated() && mq9.atHeatCycleEnd()) {
    // выполняем калибровку датчика на чистом воздухе
    mq9.calibrate();
    // выводим сопротивление датчика в чистом воздухе (Ro) в serial-порт
    Serial.print("Ro = ");
    Serial.println(mq9.getRo());
    // запускаем термоцикл
    mq9.cycleHeat();
  }
  // если прошёл интевал нагрева датчика
  // и калибровка была совершена
  if (mq9.isCalibrated() && mq9.atHeatCycleEnd()) {
    // выводим отношения текущего сопротивление датчика
    // к сопротивлению датчика в чистом воздухе (Rs/Ro)
    Serial.print("Ratio: ");
    Serial.print(mq9.readRatio());
    // выводим значения газов в ppm
    Serial.print(" LPG: ");
    Serial.print(mq9.readLPG());
    Firebase.setFloat("LPG", mq9.readLPG());
    Serial.print(" ppm ");
    Serial.print(" Methane: ");
    Serial.print(mq9.readMethane());
    Firebase.setFloat("Methane", mq9.readMethane());
    Serial.print(" ppm ");
    Serial.print(" CarbonMonoxide: ");
    Serial.print(mq9.readCarbonMonoxide());
    Firebase.setFloat("CarbonMonoxide", mq9.readCarbonMonoxide());
    Serial.println(" ppm ");
    //tone(BUZZER_PIN, 1000, 400);
    //digitalWrite(RED_LED, HIGH);
    delay(100);
    //digitalWrite(RED_LED, LOW);
    // запускаем термоцикл
    mq9.cycleHeat();
  }

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  //float celsius = thermometer.getTemperatureC();
  

  
  Firebase.setFloat("Temperature", temperature);
  Firebase.setFloat("Humidity", humidity);
  //Firebase.setFloat("Light", newLight);
  
  delay(2000);
}
