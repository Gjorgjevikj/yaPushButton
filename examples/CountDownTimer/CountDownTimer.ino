/*
    Name:       CountDownTimer.ino
    Created:	23.07.2018 15:14:18
	Updated:	08.09.2018
    Author:     Dejan Gjorgjevikj <dejan.gjorgjevikj@gmail.com>

An example program that uses the yaPushButton library, implemneting a simple countdown timer. 
Utilizes 3 buttons: +, - and S (start/stop), all conecting the pin to ground when being pressed
one output that switches a device on/off (drives a relay, active high) 
and a passive buzzer to signal end of coutdown and also as feedback 
during setting the time (or changing it while the timer is running)
and a display showing the remaining time (simulated by printing to serial in this example)

The program starts in setup mode where it enables setting the inital timer value (in minutes) by 
pressing the + and - buttons while the display is flashing.
Pressing the S button turns on the device and starts the coundown. 
In countdown mode the remaining time is shown on the display. 
Pressing the + or - button while in countdown, will increment / decrement the remaining time by 1 minute.
Pressing the S button will stop the countdown and turn off the device.
When the time runs out, the device will be turned off automatically followed by several beeps.
Then the cycle repeats (can set the time again ...)
*/

#include <yaPushButton.h>

//Push buttons
const int PLUS_PB_PIN = 12;
const int MINUS_PB_PIN = 10;
const int START_PB_PIN = 11;
// Passive buzzer
const int BUZZER_PIN = 8;
// Flashing LED
const int LED_PIN = 13;
// Device to be turned on/off
const int DEVICE_PIN = 9;

const int DISP_SIZE = 4;

// A class that simulates a 7-segment LED or LCD display with DISP_SIZE digits 
// prints on the Serial for demo purposes ...
class Display 
{
public:
	///
	void init() { changed = false; }
	void show() 
	{
		if (changed) // print on seraial only if the value has changed from the one previsly shown
		{
			for (int i = DISP_SIZE-1; i >=0 ; i--)
				Serial.print(value[i]);
			if (blinking)
				Serial.print('#');
			Serial.println();
		}
		changed = false;
	}
	void show(int v) // display an integer (last DISP_SIZE digits only)
	{
		for (int i = 0; i < DISP_SIZE; i++)
		{
			char newval = v % 10 + '0';
			if (value[i] != newval)
			{
				value[i] = newval;
				changed = true;
			}
			v /= 10;
		}
		show();
	}

	void show(char *v) // display a string (first DISP_SIZE characters only)
	{
		for (int i = DISP_SIZE - 1; i >= 0; i--)
		{
			char newval;
			if (*v)
				newval = *v++;
			else
				newval = ' ';
			if (value[i] != newval)
			{
				changed = true;
				value[i] = newval;
			}
		}
		show();
	}
	void clear() // clear the display
	{
		for (int i = 0; i < DISP_SIZE; i++)
			value[i] = ' ';
		changed = true;
		show();
	}
	void blinkOn() { blinking = true; }
	void blinkOff() { blinking = false; }
private:
	char value[DISP_SIZE];
	bool changed;
	bool blinking;
};

// Push buttons...
// A button that when hold down autorepats at first and then accelerates autorepat (up to a certain point)
PushButtonAutoAcceleratedRepeat<> ButtonPlus(PLUS_PB_PIN); // + button
// A button that when hold down autorepats at first and then enters 2-nd speed of autorepeat
PushButton2SpeedAutoRepeat<> ButtonMinus(MINUS_PB_PIN); // - button
// A button without autorepat
PushButton<> ButtonStartStop(START_PB_PIN); // start / stop button

Display display;

//Runtime globals

// Pointer to the value to be changed by up/down button 
// must be global as it is changed from an callback function with no arguments 
// ToDo - implement void * as argument to the callback so no global will be needed
int *pval;

int cntDtime; // time setted for countdown timer
unsigned long devicerun; // toal number of milliseconds the device was running
uint8_t durs = 5; // duration 1-99

// Changes the value pointed by pval, by changeBy on each call, checking for limits 
// returns false if out of bounds
template <typename T>
inline bool valChange(T *pval, T changeBy, T lowLimit = 1, T upLimit = 99)
{
	if ((changeBy > 0) && (*pval > upLimit - changeBy) || (changeBy < 0) && (*pval < lowLimit - changeBy))
		return false;
	*pval += changeBy;
	return true;
}

void pbService()
{
	if(valChange(pval, 1, 1, 99))
		tone(BUZZER_PIN, 8000, 20UL);
	else
		tone(BUZZER_PIN, 500, 40UL);
}

void mbService()
{
	if (valChange(pval, -1, 1, 99))
		tone(BUZZER_PIN, 6000, 20UL);
	else
		tone(BUZZER_PIN, 500, 40UL);
}

// Changes a value of an integer by cahangBy, checking bounds
void UpDownSet(int &v, void(*keypess_plusB)(), void(*keypess_minusB)())
{
	// set the global pointer to point to the value to be changed by pressing the +/- button
	pval = &v;
	display.clear();
	display.show(v);

	// display blinks to denote setting mode
	display.blinkOn();

	ButtonPlus.registerKeyPressCallback(keypess_plusB);
	ButtonMinus.registerKeyPressCallback(keypess_minusB);

	while (ButtonStartStop.stateChanged() != BUTTON_PRESSED) // press start button to exit seting
	{
		display.show(v);
		ButtonPlus.handle();
		ButtonMinus.handle();
		delay(1);
	}
	display.blinkOff();
}

inline void DeviceOn()
{
	digitalWrite(DEVICE_PIN, HIGH);  // turn on the device  
	Serial.println("Device turned ON");
	devicerun = millis();
}

inline void DeviceOff()
{
	digitalWrite(DEVICE_PIN, LOW);   // turn off the device 
	Serial.println("Device turned OFF");
	Serial.print("Device was running for: ");
	Serial.print(millis() - devicerun);
	Serial.println("ms.");
}

inline void LedOn()
{
	digitalWrite(LED_PIN, HIGH);   // turn the LED on 
}

inline void LedOff()
{
	digitalWrite(LED_PIN, LOW);   // turn the LED off 
}

void toogleLED()
{
	static bool isOn = false;
	isOn = !isOn; // change the isPressed
	if (isOn)
		LedOn();   // turn the LED on 
	else
		LedOff();   // turn the LED off 
}

byte CountDownMin(int &m) // in minutes
{
	unsigned long t = m * 60L * 1000L; // in milliseconds
	unsigned long start = millis();
	/*unsigned*/ long endt = start + t;
	unsigned long us; // current ms

	display.clear();
	while ((us = millis()) < endt)
	{
		unsigned long remms = endt - us; // remaining ms
		unsigned int rems = remms / 1000; // remaining s
		int remm = rems / 60 + 1; // remaining minutes

		if (ButtonPlus.isPressed() && ButtonMinus.isPressed())
		{
			// can show somtehing else on the display (like temperature) while both buttons are held pressed
			display.show("+-");
		}
		else // update the dispaly with the remaining time
		{
			if (rems < 60) // if less than 1 minute, show seconds 
			{
				display.blinkOn();
				display.show(rems + 1);
			}
			else
			{
				display.blinkOff();
				display.show(remm);
				if (rems % 2) // toggle the led on every seccond
					LedOn();
				else
					LedOff();
			}
		}
		if (ButtonStartStop.stateChanged() == BUTTON_PRESSED)
		{
			m = remm;
			return (1); // stopped
		}

		// if button + or - is pressed (buy not both simultaneosly) increase / decrease time by 1 minute
		if (!(ButtonPlus.isPressed() && ButtonMinus.isPressed()))
		{
			int premm = remm;
			pval = &remm;
			ButtonPlus.handle();
			ButtonMinus.handle();
			if (premm != remm) // increased / decreased
				valChange(&endt, (remm - premm) * 1000L * 60L, 1000 * 60L, 99000 * 60L);
		}
	}
	return(0); // finshed regularly
}

byte CountEnded() 
{
	display.show("CE");
	display.blinkOff();
	unsigned long updperiod = 100, lastupd = millis();
	uint8_t brightneidxchange = 1, brightneidx = 0;

	while (ButtonStartStop.stateChanged() != BUTTON_RELEASED
		&& ButtonPlus.stateChanged() != BUTTON_RELEASED
		&& ButtonMinus.stateChanged() != BUTTON_RELEASED)
	{
		if (millis() - lastupd > updperiod)
		{
			lastupd = millis();
			if (brightneidx == 0)
				brightneidxchange = 1;
			else if (brightneidx == 4)
				brightneidxchange = -1;
			brightneidx += brightneidxchange;
		}

		if (ButtonStartStop.isPressed() || ButtonPlus.isPressed() || ButtonMinus.isPressed())
		{
			display.show("--");
			tone(BUZZER_PIN, 1000, 200UL);
		}
		delay(1);
	}
	return(0); // finshed regularly
}


void setup()
{
	Serial.begin(9600);
	delay(20);
	Serial.println("Start.");
	pinMode(LED_PIN, OUTPUT);
	pinMode(DEVICE_PIN, OUTPUT);
	pinMode(BUZZER_PIN, OUTPUT);
	digitalWrite(DEVICE_PIN, LOW);
	
	display.init();
	ButtonPlus.init();
	ButtonMinus.init();
	ButtonStartStop.init();

	cntDtime = durs;
	tone(BUZZER_PIN, 2000, 200);
	delay(200);
}

void loop()
{
	UpDownSet(cntDtime, pbService, mbService);
	durs = cntDtime;
	Serial.print("Seted time: ");
	Serial.print(cntDtime);
	Serial.println(" min.");

	delay(100);
	display.show(cntDtime);
	tone(BUZZER_PIN, 2000, 100UL);

	DeviceOn();
	byte r = CountDownMin(cntDtime);
	DeviceOff();

	if (r == 1) // stopped prematurely
	{
		tone(BUZZER_PIN, 500, 1200UL);
		display.show("Stop");
		// wait for release if the button has not been released yet
		while (ButtonStartStop.stateChanged() != BUTTON_RELEASED)
			;
	}
	// else if (r == 2) // over temperature protection or other events
	//{
	//}
	else
	{
		display.show("end ");
		for (int i = 0; i < 4; i ++)
		{
			tone(BUZZER_PIN, 1000, 1500UL);
			for (int j = 0; j < 5; j++)
			{
				display.show(" end");
				delay(500);
			}
			if (ButtonStartStop.stateChanged() == BUTTON_RELEASED)
				break;
		}
	}
}
