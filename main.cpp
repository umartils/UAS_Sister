#define BLYNK_TEMPLATE_ID "TMPL6u2BJBTn3"
#define BLYNK_TEMPLATE_NAME "Project01"
#define BLYNK_AUTH_TOKEN "s41lar4aF097xSGG5FFo2cN5meBNjmJN"

#define BLYNK_PRINT Serial

#include <WiFi.h> 
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//SCA21, SCL22
//Relay Pin = 2
//Sensor Pin = 4
//Buzzer pin = 19

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "umarumar";  // type your wifi name
char pass[] = "umar1234567";  // type your wifi password

BlynkTimer timer;

#define DHTPIN 4 //Connect Out pin to D2 in NODE MCU
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

int green_led = 5;
int red_led = 18;

int relay_pin = 2;
int buzzer_pin = 19;


float humDHT = 0;
float tempDHT = 0;
int threshold = 20;

int buzzerState = LOW;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{   
  Serial.begin(9600);

  pinMode(relay_pin,OUTPUT);
  digitalWrite(relay_pin, LOW);

  pinMode(buzzer_pin, OUTPUT);

  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);

  
  Blynk.begin(auth, ssid, pass);
  dht.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
}

BLYNK_WRITE(V7)
{
  threshold = param.asInt();
  Serial.print(" The Threshhold thresholdue is: ");
  Serial.println(threshold);
  Serial.println();
}

void loop()
{
  Blynk.run();
  timer.run();

  humDHT = dht.readHumidity();

  tempDHT = dht.readTemperature();

  if (isnan(humDHT) || isnan(tempDHT))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print(F("Temperature: "));
  Serial.print(tempDHT);
  Serial.print(F("°C "));
  Serial.println();
  Serial.print(F("Humidity: "));
  Serial.print(humDHT);
  Serial.print(F("%"));
  Serial.println();
  
  Serial.println("***********************");
  Serial.println();

  if (tempDHT > threshold) {
    digitalWrite(relay_pin, HIGH);  // Kipas tetap menyala
    digitalWrite(green_led, HIGH);
    digitalWrite(red_led, LOW);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Fan  : ON");
    Blynk.virtualWrite(V2, HIGH); 
  } else {
    digitalWrite(relay_pin, LOW);   // Matikan kipas jika suhu di bawah atau sama dengan 32 derajat
    digitalWrite(green_led, LOW);
    digitalWrite(red_led, HIGH);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Fan  : OFF");
    Blynk.virtualWrite(V2, LOW); 
  }

  // Logika untuk mengaktifkan buzzer sekali ketika suhu melebihi ambang batas
  if (tempDHT >= threshold && buzzerState == LOW) {
    tone(buzzer_pin, 2000);
    delay(100);
    noTone(buzzer_pin);
    
    // Set flag agar buzzer tidak berbunyi lagi sampai kondisi berubah
    buzzerState = HIGH;
  } else if (tempDHT < threshold) {
    // Reset flag saat suhu turun di bawah atau sama dengan 32 derajat
    buzzerState = LOW;
  }

  // Tampilkan suhu dan kelembaban pada layar OLED
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 24);
  display.print("Temp : ");
  display.print(tempDHT);
  display.print(" C");

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 34);
  display.print("Hum  : ");
  display.print(humDHT);
  display.print(" %");

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 48);
  display.print("KELOMPOK 8");

  display.display();
  delay(2000);  // Delay untuk memberikan waktu membaca layar
  display.clearDisplay();

  Blynk.virtualWrite(V0, tempDHT);
  Blynk.virtualWrite(V1, humDHT);
  

}
 