#include <yaPushButton.h>

// Passive buzzer: D8
const int BUZZER_PIN = 8;

// Active low push buttons - all connecting to GND
const int PB_PIN_PLUS = 12;
const int PB_PIN_MINUS = 11;
const int PB_PIN_START = 10;

const int LED_PIN = 13;
const int DEVICE_PIN = 9;

// A simple button with debouncing (without autorepeat)
PushButton<> Button1(PB_PIN_MINUS); 
PushButton<> Button2(PB_PIN_START, 500); // this button will react after being depressed at least 0.5s
PushButton<> Button3(PB_PIN_PLUS); 

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
}

void toggleDevice()
{
  static bool deviceState = false; // device is initially off
  deviceState = !deviceState;
  if(deviceState)
  {
    digitalWrite(DEVICE_PIN, HIGH);
    Button3.setDebounceDelay(750); // extend debounce on button3
  }
  else
  {
    digitalWrite(DEVICE_PIN, LOW);
    Button3.setDebounceDelay(50);
  }
}

void loop() 
{
  // light the led while any of the buttons is being held down
  if(Button1.isPressed() || Button2.isPressed() || Button3.isPressed())
    digitalWrite(LED_PIN, HIGH);
  else    
    digitalWrite(LED_PIN, LOW);

  byte button1 = Button1.stateChanged(); // has the state of button1 changed 
  if(button1 == BUTTON_PRESSED)
  {
    Serial.println("Button#1 PRESSED");
    tone(BUZZER_PIN, 1000, 100);
  }
  else if(button1 == BUTTON_RELEASED)
  {
    Serial.println("Button#1 RELEASED");
    tone(BUZZER_PIN, 500, 100);
  }

  if(Button2.stateChanged() == BUTTON_PRESSED) // button 2 will turn on the LED connected to DEVICE_PIN, pressing it again will turn the device off
  {
      Serial.println("Button#2 PRESSED"); 
      toggleDevice(); // while the device is turned on the delay on button3 is prolonged
  }
  
  switch(Button3.stateChanged()) 
  {
    case BUTTON_PRESSED:
      Serial.println("Button#3 PRESSED"); 
      tone(BUZZER_PIN, 4000, 100);
      break; // while the device is turned on the delay on button3 is prolonged
    case BUTTON_RELEASED:
      Serial.println("Button#3 RELEASED"); 
      tone(BUZZER_PIN, 2000, 100);
      break;
  }
  
  // do whatever else is needed in the loop
  delay(1);
}
