#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

#include "led.h"

int LEDclass::speed;

LEDclass::LEDclass(int Pin, int num)
{
	pin = Pin;
	number = num;
	
	pinMode(pin, OUTPUT);
	
	onOff = false;
	value = 0;
	targetValue = 0;
	
	speed = 10;
}

void LEDclass::turnOn()
{
	setValue(lastTargetValue);
}

void LEDclass::turnOff()
{
	setValue(0);
}

void LEDclass::setValue(int Value)
{
	targetValue = Value;
	
	if(targetValue >= 90)
	{
		onOff = true;
		lastTargetValue = targetValue;
	}
	else
	{
		onOff = false;
	}
}

void LEDclass::set()
{
	if((targetValue > value) && (value < 1000) && onOff)
		value = value + speed;
	else if((targetValue < value) && (value > 90))
		value = value - speed;
	
	analogWrite(pin, pow(2.0,value/100.0));
}

void LEDclass::setNow()
{
	value = targetValue;
	
	analogWrite(pin, pow(2.0,value/100.0));
}
