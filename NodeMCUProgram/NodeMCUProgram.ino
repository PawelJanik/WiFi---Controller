#include "config.h"

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "setup.h"
#include "led.h"

WiFiClient espClient;
PubSubClient client(espClient);

LEDclass led[4] = {{D4, 1}, {D3, 2}, {D2, 3}, {D1, 4}};

OneWire oneWire(D6);
DallasTemperature sensors(&oneWire);

const int pinWallSwitch = D5;
bool switchState;

bool lightState;

unsigned long ledTimer = 0;
unsigned long timer10s = 0;
unsigned long timer1m = 0;

int bytesToInt(unsigned char* b, unsigned length)
{
	int val = 0;
	int j = 0;
	for (int i = length-1; i >= 0; --i)
	{
		val += (b[i] - '0') * pow(10,j);
		j++;
	}
	return val;
}

void reconnect() 
{
	Serial.print("Attempting MQTT connection...");
	if (client.connect(controllerName, mqttLogin, mqttPasswd))
	{
		Serial.println("connected");
		
		client.subscribe("home/myRoom/light/1");
		client.subscribe("home/myRoom/light/1/level");
		
		client.subscribe("home/myRoom/light/2");
		client.subscribe("home/myRoom/light/2/level");
		
		client.subscribe("home/myRoom/light/3");
		client.subscribe("home/myRoom/light/3/level");
		
		client.subscribe("home/myRoom/light/4");
		client.subscribe("home/myRoom/light/4/level");
		
		client.subscribe("home/myRoom/light/0");
		client.subscribe("home/myRoom/light/0/level");
		
		digitalWrite(D10, HIGH);
	} 
	else
	{
		Serial.print("failed, rc=");
		Serial.print(client.state());
		Serial.println(" try again in 1 seconds");
		delay(1000);
		digitalWrite(D10, LOW);
	}
}

void callback(char * topic, byte* payload, unsigned int length) 
{
	if(strcmp(topic,"home/myRoom/light/1")==0)
	{
		if ((char)payload[0] == '1') 
		{
			led[0].turnOn();
			lightState = true;
		}
		else 
		{
			led[0].turnOff();
			lightState = false;
		}
	}
	
	if(strcmp(topic,"home/myRoom/light/2")==0)
	{
		if ((char)payload[0] == '1') 
		{
			led[1].turnOn();
			lightState = true;
		}
		else 
		{
			led[1].turnOff();
			lightState = false;
		}
	}
	
	if(strcmp(topic,"home/myRoom/light/3")==0)
	{
		if ((char)payload[0] == '1') 
		{
			led[2].turnOn();
			lightState = true;
		}
		else 
		{
			led[2].turnOff();
			lightState = false;
		}
	}
	
	if(strcmp(topic,"home/myRoom/light/4")==0)
	{
		if ((char)payload[0] == '1') 
		{
			led[3].turnOn();
			lightState = true;
		}
		else 
		{
			led[3].turnOff();
			lightState = false;
		}
	}
	
	if(strcmp(topic,"home/myRoom/light/0")==0)
	{
		if ((char)payload[0] == '1') 
		{
			for(int i = 0; i < 4; i++)
				led[i].turnOn();
			
			lightState = true;
		} 
		else 
		{
			for(int i = 0; i < 4; i++)
				led[i].turnOff();
			
			lightState = false;
		}
	}
	
	if(strcmp(topic,"home/myRoom/light/1/level")==0)
	{
		int value = bytesToInt(payload,length);
		Serial.println(value);
		led[0].setValue(value);
		
		if(value > 0)
			lightState = true;
		else
			lightState = false;
	}
	
	if(strcmp(topic,"home/myRoom/light/2/level")==0)
	{
		int value = bytesToInt(payload,length);
		Serial.println(value);
		led[1].setValue(value);
		
		if(value > 0)
			lightState = true;
		else
			lightState = false;
	}
	
	if(strcmp(topic,"home/myRoom/light/3/level")==0)
	{
		int value = bytesToInt(payload,length);
		Serial.println(value);
		led[2].setValue(value);
		
		if(value > 0)
			lightState = true;
		else
			lightState = false;
	}
	
	if(strcmp(topic,"home/myRoom/light/4/level")==0)
	{
		int value = bytesToInt(payload,length);
		Serial.println(value);
		led[3].setValue(value);
		
		if(value > 0)
			lightState = true;
		else
			lightState = false;
	}
	
	if(strcmp(topic,"home/myRoom/light/0/level")==0)
	{
		int value = bytesToInt(payload,length);
		Serial.println(value);
		
		for(int i = 0; i < 4; i++)
			led[i].setValue(value);
		
		if(value > 0)
		{
			lightState = true;
		}
		else
		{
			lightState = false;
		}
	}
}

void setup() 
{
	pinMode(D9, OUTPUT);	//wifi
	pinMode(D10, OUTPUT);	//mqtt
	
	Serial.begin(115200);
	Serial.println("Booting");
	Serial.println("Connecting to WiFi...");
	
	WIFIsetup();
	
	digitalWrite(D9, HIGH);
	
	Serial.println("WiFi Ready");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	
	Serial.println("OTA setup...");
	OTAsetup();
	Serial.println("OTA ready");

	Serial.println("MQTT setup...");
	client.setServer(mqttIP, mqttPort);
	client.setCallback(callback);
	reconnect();
	Serial.println("MQTT ready");
	
	pinMode(D5, INPUT_PULLUP);
	switchState = (digitalRead(D5));
	lightState = false;
	
	sensors.begin();
	
	//LEDclass::speed = 1;
}

void loop() 
{
	if (!client.connected()) 
	{
		reconnect();
		
		if(digitalRead(D5) != switchState)
		{
			switchState = digitalRead(D5);
		

			if(lightState == false)
			{
				for(int i = 0; i < 4; i++)
					led[i].turnOn();
				
				lightState = true;
			}
			else
			{
				for(int i = 0; i < 4; i++)
					led[i].turnOff();
				
				lightState = false;
			}
			
			for(int i = 0; i < 4; i++)
					led[i].setNow();
		}
	}
	else
	{
		client.loop();
	}

	ArduinoOTA.handle();
	
	if((millis() - ledTimer) > 10)
	{
		ledTimer = millis();

		for(int i = 0; i < 4; i++)
			led[i].set();
	}
	
	if((millis() - timer10s) > 10000)
	{
		timer10s = millis();
		client.publish("home/controllers/0/status", "ok");
	}
	
	if((millis() - timer1m) > 60000)
	{
		timer1m = millis();
		
		sensors.requestTemperatures();
		client.publish("home/sensors/temperature/0", String(sensors.getTempCByIndex(0)).c_str());
		client.publish("home/sensors/temperature/1", String(sensors.getTempCByIndex(1)).c_str());
		client.publish("home/sensors/temperature/2", String(sensors.getTempCByIndex(2)).c_str());
	}
	
	if(digitalRead(D5) != switchState)
	{
		switchState = digitalRead(D5);
		
		
		if(lightState == false)
		{
			client.publish("home/myRoom/wallSwitch", "1");
		}
		else
		{
			client.publish("home/myRoom/wallSwitch", "0");
		}
	}
}
