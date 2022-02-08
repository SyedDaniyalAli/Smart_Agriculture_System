#include <BlynkSimpleEsp8266.h> // Import the Bylnk library to use its function which will facilitate the data transfer from the app
#include "DHT.h"
// Developed by SDA
// Define Trig and Echo pin for Ultra Sonic:
#define trigPin D0
#define echoPin D1
#define buzzer D2
#define DHTPIN D3     // Digital pin connected to the DHT sensor
#define waterPumpPin D4
#define waterPumpPin2 D5
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define soilMoistReaderPin D7
#define soilPhReaderPin D8
int soilSensorPin = A0;  /* Soil moisture sensor O/P pin */


// Define variables for Ultra Sonic:
int waterLevel = 2;
int moistPer = 25;
DHT dht(DHTPIN, DHTTYPE);

unsigned long sendDataPrevMillis = 0;
unsigned long updateDataPrevMillis = 0;

// Struct for humidity~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct DHTData {
  float humidity;
  float temp;
  float heatIndex;
};

DHTData dhtData;



//Wifi Settings~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//char auth[] ="BvaY5wlq1Qcb9f8uA-L_qCU5vBZp65N2";    // N Authentication code sent by Blynk
//char auth[] = "hmw_WPi_sOHF7F47PpU-sx-3b9Mp_aHW"; // D //Authentication code sent by Blynk
char auth[] = "NBAPQU7zUDyTv4_YKOm1WbM1TzX6dUrf"; // S Authentication code sent by Blynk

char ssid[] = "DreamNetSDA";                       //WiFi SSID
char pass[] = "Daniyal444";                       //WiFi Password
//char ssid[] = "M.Sajid";                       //WiFi SSID
//char pass[] = "0987654321";                       //WiFi Password


void setup() {

  Serial.begin(9600);

  Serial.println("Connecting to Wifi");

  Blynk.begin(auth, ssid, pass);
  Serial.println("Wifi connected");

  //  Start DHT Sensor
  dht.begin();
  Serial.println("dht connected");

  // Define inputs and outputs:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(waterPumpPin, OUTPUT);
  pinMode(waterPumpPin2, OUTPUT);
  pinMode(soilMoistReaderPin, OUTPUT);
  pinMode(soilPhReaderPin, OUTPUT);
  digitalWrite(waterPumpPin, HIGH);
  digitalWrite(waterPumpPin2, HIGH);


}

void loop() {

  Blynk.run();

  if (millis() - updateDataPrevMillis > 600) {
    updateDataPrevMillis = millis();

    //    Getting Water level
    waterLevel = getWaterLevel();

    //  Getting Moist percentage
    moistPer = getMoistLevel();

  }


  if (millis() - sendDataPrevMillis > 15000) {

    sendDataPrevMillis = millis();

    //  Getting Temperature, Humidity and HeatIndex
    dhtData = getTemperature_Humidity_HIC();
    //  Serial.println(dhtData.humidity);
    //  Serial.println(dhtData.temp);
    //  Serial.println(dhtData.heatIndex);

    float phLevel = getPhLevel();

    Blynk.virtualWrite(V1, dhtData.humidity);  //V1 is for humidity
    Blynk.virtualWrite(V2, dhtData.temp);  //V2 is for temperature
    Blynk.virtualWrite(V3, dhtData.heatIndex);  //V3 is for heat index
    Blynk.virtualWrite(V4, phLevel);  //V3 is for heat index
  }


  //checking water level~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (waterLevel > 2 && waterLevel != 0)
  {
    digitalWrite(waterPumpPin2, LOW); // Pump2 on
    Blynk.virtualWrite(V4, waterLevel);  //V3 is for Water Level
  }
  else if (waterLevel != 0 && waterLevel <= 2)
  {
    digitalWrite(waterPumpPin2, HIGH); // Pump2 off
    Blynk.virtualWrite(V4, waterLevel);  //V3 is for Water Level
  }


  //checking moist level~~~~~~~~~~~~~~~~~~~~~~~~~~~

  if (moistPer < 25) {

    // Calculate the distance:
    if (waterLevel <= 4)
    {
      digitalWrite(waterPumpPin, LOW); // Pump on
    }

    Blynk.virtualWrite(V5, moistPer);  //V3 is for moistPer
  }
  else if (moistPer > 35)
  {
    digitalWrite(waterPumpPin, HIGH); // Pump off
    Blynk.virtualWrite(V5, moistPer);  //V3 is for moistPer
  }


  // Buzzer by checking level~~~~~~~~~~~~~~~~~~~
  // Calculate the distance:
  //  waterLevel = getWaterLevel();
  //  if (waterLevel <= 5) {
  //    tone(buzzer, 1000);
  //    Serial.println("Buzzer on");
  //  }
  //  else
  //  {
  //    noTone(buzzer);
  //    Serial.println("Buzzer off");
  //  }
  //  delay(1000);

}


struct DHTData getTemperature_Humidity_HIC()
{
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  dhtData.humidity = h;
  dhtData.temp = t;
  dhtData.heatIndex = hic;


  //  Serial.print(F(" Humidity: "));
  //  Serial.print(h);
  //  Serial.print(F("%  Temperature: "));
  //  Serial.print(t);
  //  Serial.print(F("C "));
  //  Serial.print(F("  Heat index: "));
  //  Serial.print(hic);
  //  Serial.print(F("C "));

  return dhtData;
}

float getMoistLevel()
{
  digitalWrite(soilMoistReaderPin, HIGH); // Turn D7 On
  digitalWrite(soilPhReaderPin, LOW); // Turn D8 Off

  float moisture_percentage;
  int sensor_analog;
  sensor_analog = analogRead(soilSensorPin);
  moisture_percentage = ( 100 - ( (sensor_analog / 1023.00) * 100 ) );
  Serial.print("Moisture Percentage = ");
  Serial.print(moisture_percentage);
  Serial.print("%\n\n");
  return moisture_percentage;
}


float getPhLevel()
{
  digitalWrite(soilMoistReaderPin, LOW); // Turn D8 Off
  digitalWrite(soilPhReaderPin, HIGH); // Turn D7 On

  float soil_ph_value;
  int sensor_analog;
  sensor_analog = analogRead(soilSensorPin);
  soil_ph_value = ( 100 - ( (sensor_analog / 1023.00) * 100 ) );
  Serial.print("PH of Soil Sensor: ");
  Serial.println(sensor_analog/100);
  return sensor_analog/100;
}



int getWaterLevel()
{
  long duration;
  int distance;
  int distanceInch;

  // Clear the trigPin by setting it LOW:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);

  // Read the echoPin. This returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  distanceInch = duration * 0.0133 / 2;

  Serial.print("Ultrasonic Distance: ");
  Serial.println(distanceInch);
  delay(250);


  return distanceInch;
}
