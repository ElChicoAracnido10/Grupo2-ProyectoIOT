#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000);

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C y dimensiones del LCD

// credenciales de wifi

// credenciales de adafruit

int SERVO_PIN = D3; // Pin donde está conectado el servo
int CLOSE_ANGLE = 0; // Ángulo de cerrado del servo
int OPEN_ANGLE = 180; // Ángulo de apertura del servo
int hh, mm, ss;
int feed_hour = 0;
int feed_minute = 0;

// Configuración de clientes MQTT y WiFi
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

// Suscripción al feed onoff
Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/onoff");
boolean feed = true; // Condición para la alarma

void setup()
{
  Serial.begin(9600);
  timeClient.begin();
  Wire.begin(D2, D1); // Inicializar I2C
  
  // Inicializar LCD
  lcd.begin(16, 2);
  lcd.init(); 
  lcd.backlight(); 
  lcd.clear(); 

  // Conectar a WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("OK!");

  // Suscribirse al feed onoff
  mqtt.subscribe(&onoff);
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_ANGLE);
}

void loop()
{
  MQTT_connect();
  timeClient.update();
  hh = timeClient.getHours();
  mm = timeClient.getMinutes();
  ss = timeClient.getSeconds();

  // Mostrar la hora actual
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (hh > 12)
  {
    hh = hh - 12;
    lcd.print(hh);
    lcd.print(":");
    lcd.print(mm);
    lcd.print(":");
    lcd.print(ss);
    lcd.print(" PM  ");
  }
  else
  {
    lcd.print(hh);
    lcd.print(":");
    lcd.print(mm);
    lcd.print(":");
    lcd.print(ss);
    lcd.print(" AM  ");
  }

  // Mostrar la hora de alimentación
  lcd.setCursor(0, 1);
  lcd.print("Feed Time: ");
  lcd.print(feed_hour);
  lcd.print(':');
  lcd.print(feed_minute);

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    if (subscription == &onoff)
    {
      Serial.println((char*) onoff.lastread);

      if (!strcmp((char*) onoff.lastread, "ON"))
      {
        open_door();
        delay(1000);
        close_door();
      }
      if (!strcmp((char*) onoff.lastread, "Morning"))
      {
        feed_hour = 10;
        feed_minute = 30;
      }
      if (!strcmp((char*) onoff.lastread, "Afternoon"))
      {
        feed_hour = 1;
        feed_minute = 30;
      }
      if (!strcmp((char*) onoff.lastread, "Evening"))
      {
        feed_hour = 6;
        feed_minute = 30;
      }
    }
  }

  if (hh == feed_hour && mm == feed_minute && feed == true)
  {
    open_door();
    delay(1000);
    close_door();
    feed = false;
  }
}

void MQTT_connect()
{
  int8_t ret;

  if (mqtt.connected())
  {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  {
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0)
    {
      while (1);
    }
  }
}

void open_door()
{
  servo.write(OPEN_ANGLE);
}

void close_door()
{
  servo.write(CLOSE_ANGLE);
}
