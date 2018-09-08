//#define NDEBUG
#include <yaPushButton.h>

// Passive buzzer: D8
const int BUZZER_PIN = 8;

// Active low push buttons - all connecting to GND
const int PB_PIN_PLUS = 12;
const int PB_PIN_MINUS = 10;
const int PB_PIN_START = 11;

const int LED_PIN = 13;
const int DEVICE_PIN = 9;

void Button1Callback()
{
	tone(BUZZER_PIN, 1000, 10);
	Serial.print('1');
}

void Button2Callback()
{
	tone(BUZZER_PIN, 2000, 10);
	Serial.print('2');
}

void Button3Callback()
{
	tone(BUZZER_PIN, 4000, 10);
	Serial.print('3');
}

// A button with auto-repaet
PushButtonAutoRepeat<> Button1(PB_PIN_MINUS, Button1Callback);
PushButton2SpeedAutoRepeat<> Button2(PB_PIN_START, Button2Callback);
PushButtonAutoAcceleratedRepeat<> Button3(PB_PIN_PLUS, Button3Callback);

void setup() 
{
  Serial.begin(9600);
  delay(200);
  Serial.println("Start.");

  pinMode(LED_PIN, OUTPUT);
  pinMode(DEVICE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(DEVICE_PIN, LOW);

  for(int i =0; i<3; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 50);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  Button1.init();
  Button2.init();
  Button3.init();
  //Button2.setRepeatAccelerateDelay(10UL); // will cause an assert to fail
}

void loop() 
{
  // light the led while any of the buttons is beeing held down
  if(Button1.isPressed() || Button2.isPressed() || Button3.isPressed())
    digitalWrite(LED_PIN, HIGH);
  else    
    digitalWrite(LED_PIN, LOW);

  if (Button1.stateChanged() == BUTTON_RELEASED ||
	  Button2.stateChanged() == BUTTON_RELEASED ||
	  Button3.stateChanged() == BUTTON_RELEASED)
	  Serial.println('<');

  Button1.handle();
  Button2.handle();
  Button3.handle();

  // do whatever else is needed in the loop
  delay(1);
}


// handle diagnostic informations given by assertion and abort program execution:
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
	// transmit diagnostic informations through serial link. 
	Serial.print(F("Assertion @ line# "));
	Serial.print(__lineno, DEC);
	Serial.print(F(" failed: ("));
	Serial.print(__sexp);
	Serial.print(F(") in "));
	Serial.println(__func);
//	Serial.println(__file);
	Serial.flush();
	// abort program execution.
	//abort();
	pinMode(LED_PIN, OUTPUT);
	while (true)
	{
		digitalWrite(LED_PIN, HIGH);
		delay(100);
		digitalWrite(LED_PIN, LOW);
		delay(100);
	}
}
