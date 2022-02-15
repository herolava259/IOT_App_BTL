#include<ESP8266WiFi.h>

#include<DHT.h>

#define DHTTYPE DHT11

const char* ssid = 'LE SON';

const char* password = '0766709993';

WiFiServer server(80);

const int DHTPin = 5;

DHT dht(DHTPin, DHTTYPE);

static char celsiusTemp[7];

static char fahrenheitTemp[7];

static char humidityTemp[7];

void setup()

{

  Serial.begin(115200);

  delay(10);

  dht.begin();

  Serial.println();

  Serial.print('Dang ket noi den mang… ');

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)

  {

    delay(500);

    Serial.print('.');

  }

  Serial.println('');

  Serial.println('Da ket noi WiFi');

  server.begin();

  Serial.println('Web server dang khoi dong. Vui long doi dia chi IP…');

  delay(1000);

  Serial.println(WiFi.localIP());

}

void loop()

{

  WiFiClient client = server.available();

  if (client)

  {

    Serial.println('Co client moi');

    boolean blank_line = true;

    while (client.connected())

    {

      if (client.available())

      {

        char c = client.read();

        if (c == '\n' && blank_line)

        {

          float h = dht.readHumidity();

          float t = dht.readTemperature();

          float f = dht.readTemperature(true);

          if (isnan(h) || isnan(t) || isnan(f))

          {

            Serial.println('Khong the doc du lieu tu cam bien DHT!');

            strcpy(celsiusTemp, 'Failed');

            strcpy(fahrenheitTemp, 'Failed');

            strcpy(humidityTemp, 'Failed');

          }

          else

          {

            float hic = dht.computeHeatIndex(t, h, false);

            dtostrf(hic, 6, 2, celsiusTemp);

            float hif = dht.computeHeatIndex(f, h);

            dtostrf(hif, 6, 2, fahrenheitTemp);

            dtostrf(h, 6, 2, humidityTemp);

          }

          client.println('HTTP/1.1 200 OK');

          client.println('Content-Type: text/html');

          client.println('Connection: close');

          client.println();

          client.println('<!DOCTYPE HTML>');

          client.println('<html>');

          client.println('<h1>ESP8266 & DHT11</h1>');

          client.println('<head></head><body><h2>Nhiet do va do am</h2><h3>Nhiet do theo do C: ');

          client.println(celsiusTemp);

          client.println('*C</h3><h3>Nhiet do theo do F: ');

          client.println(fahrenheitTemp);

          client.println('*F</h3><h3>Do am: ');

          client.println(humidityTemp);

          client.println('%</h3><h3>');

          client.println('<h2>dientuadenz.com</h2>');

          client.println('</body></html>');

          break;

        }

        if (c == '\n')

        {

          blank_line = true;

        }

        else if (c != '\r')

        {

          blank_line = false;

        }

      }

    }

    delay(1);

    client.stop();

    Serial.println('Ngat ket noi client.');

  }

}
