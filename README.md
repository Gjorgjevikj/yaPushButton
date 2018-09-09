# yaPushButton

**Y**et **A**nother **Push** **Button** library for Arduino (supports active low or high buttons, debouncing, callback, autorepeat, ...)

4 template classes:

## PushButton class

Simple push button class that supports debouncing. `stateChanged()` returns press and release events implementing debouncing.

  ```C
  PushButton< bool ActiveHighOrLow >
  {
    PushButton(pbPin, DebounceDelay);
    void init(); // called to initialize 
    void setDebounceDelay(delay); // Sets the debouncing delay in milliseconds 
    unsigned long getDebounceDelay(); // Returns the debouncing delay in milliseconds 
    bool isPressed(); // Is the button being (held) pressed in the moment
    byte stateChanged(); // Detect state change of a button, returns BUTTON_PRESSED, BUTTON_RELEASED or BUTTON_NOCHANGE but only after the debounce delay time has passed 
  }
  ```

Examle:

  ```C
  PushButton<> Button1(PB_PIN); 
  . . .
  Button1.init();
  . . .
  void loop() 
  {
    . . .
    byte button1 = Button1.stateChanged(); // has the state of button1 changed 
    if(button1 == BUTTON_PRESSED)
      Serial.println("Button#1 PRESSED");
    if(button1 == BUTTON_RELEASED)
        Serial.println("Button#1 RELEASED");
    . . .
  }
  ```

## PushButtonAutoRepeat class

Implements auto-repeat when the button is being held depressed. Callback function should be registered in order to handle the auto-repeated events. The delay before the auto-repeat begins and the speed at which repeating is generated is configurable. `handle()` must be called repeatedly in a loop to service auto-repeating.

  ```C
  PushButtonAutoRepeat< bool ActiveHighOrLow > // derived from PushButton
  {
    PushButtonAutoRepeat(pbPin, void(*KeyPressCallBackFunction)(), RepeatDelay, AutoRepeatingPeriod, DebounceDelay);
    * KeyPressCallBackFunction - pointer to the function to be called on each autorepeat
    * RepeatDelay - wait for milliseconds befor starting to autorepeat
    * AutoRepeatingPeriod - controls the speed of autorepeat, calls the callback function every period milliseconds while the buttonis held pressed
    void registerKeyPressCallback(void(*keyPressFunction)());
    void setRepeatDelay(RepeatDelay);
    unsigned long getRepeatDelay();
    void setRepeatPeriod(AutoRepeatingPeriod);
    unsigned long getRepeatPeriod() ;
    unsigned long heldDown(); // For how long the button is being held pressed
    void handle(); // To be called repeatedly in a loop (services auto-repeating calls)
  }
  ```

Examle:

  ```C
  void Button1Callback()
  {
    tone(BUZZER_PIN, 1000, 10);
    Serial.print('1');
  }

  PushButtonAutoRepeat<> Button1(PB_PIN_MINUS, Button1Callback);
  . . .
  Button1.init();
  . . .
  void loop() 
  {
    if (Button1.stateChanged() == BUTTON_RELEASED)
      Serial.println('<');

    Button1.handle();

    // do whatever else is needed in the loop
  }
  ```

## PushButton2SpeedAutoRepeat class

Implements acceleration of auto-repeat - when the button is being held depressed long enough, the rate of generating key-presses enters 2nd speed.

  ```C
  PushButton2SpeedAutoRepeat< bool ActiveHighOrLow > // derived from PushButtonAutoRepeat - implements accelerated speed of auto-repeat
  {
    PushButton2SpeedAutoRepeat(pbPin, void(*KeyPressCallBackFunction)(), RepeatDelay, AutoRepeatingPeriod, RepeatAccelerateDelay, RepeatPeriodAcc, DebounceDelay);
    * RepeatAccelerateDelay - delay in millisecond before acceleration (entering 2nd speed) begins
    * RepeatPeriodAcc - regulates the speed of auto-repeat in 2nd speed 
    void setRepeatAccelerateDelay(RepeatAccelerateDelay);
    unsigned long getRepeatAccelerateDelay();
    void setRepeatPeriodAcc(RepeatPeriodAcc);
    unsigned long getRepeatPeriodAcc();
  }
  ```

## PushButtonAutoAcceleratedRepeat class

Implements gradual acceleration of auto-repeat as the button is being held depressed longer (up to a certain level).

  ```C
  PushButtonAutoAcceleratedRepeat< bool ActiveHighOrLow > // derived from PushButtonAutoRepeat - implements continous acceleration (to a limit) of auto-repeat
  {
    PushButtonAutoAcceleratedRepeat(pbPin, void(*KeyPressCallBackFunction)(), RepeatDelay, AutoRepeatingPeriod, RepeatDelayAcc, RepeatAcc, repeatMinPeriod, DebounceDelay);
    * RepeatDelayAcc - delay in milliseconds before acceleration begins
    * RepeatAcc - rate of acceleration
    * repeatMinPeriod - when to stop further acceleration (when generating new keypress every repeatMinPeriod milliseconds is reached)
    void setRepeatAccelerateDelay(RepeatAccelerateDelay);
    unsigned long getRepeatAccelerateDelay();
    void setRepeatPeriodAcc(RepeatPeriodAcc);
    unsigned long getRepeatPeriodAcc();
  }
  ```

See the examples in the examples\ folder.

Comments are velcome.

