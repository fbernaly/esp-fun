struct LedController : Service::LightBulb {  // create a derived class from the HomeSpan LightBulb Service

  int ledPin;                 // this variable stores the pin number defined for this LED
  int pushButtonPin;          // pin with pushbutton to turn on/off LED
  SpanCharacteristic *power;  // here we create a generic pointer to a SpanCharacteristic named "power" that we will use below

  // Next we define the constructor for LED.  Note that it takes two arguments: ledPin and powerPin.

  LedController(int ledPin, int pushButtonPin)
    : Service::LightBulb() {
    power = new Characteristic::On();     // this is where we create the On Characterstic.  Save this in the pointer created above, for use below
    this->ledPin = ledPin;                // save led pin number
    this->pushButtonPin = pushButtonPin;  // save power pushbutton pin number
    pinMode(ledPin, OUTPUT);              // set the mode for ledPin to be an OUTPUT (standard Arduino function)
    digitalWrite(ledPin, LOW);
    new SpanButton(pushButtonPin);  // create new SpanButton to control power using pushbutton on pin number "powerPin"
  }

  // Over-ride the default update() method with instructions that actually turn on/off the LED.  Note update() returns type boolean

  boolean update() {
    LOG1("Got update from HomeKit");
    LOG1("\n");
    digitalWrite(ledPin, power->getNewVal());  // use a standard Arduino function to turn on/off ledPin based on the return of a call to power->getNewVal()
    return (true);                             // return true to indicate the update was successful (otherwise create code to return false if some reason you could not turn on the LED)
  }

  void loop() {
    if (power->getVal() && power->timeVal() > (60 * 60000)) {  // check that power is true, and that time since last modification is greater than 60 mins
      power->setVal(false);
      digitalWrite(ledPin, LOW);  // set power to false
      LOG1("Turn off after timeout");
      LOG1("\n");
    }
  }

  // Here is the button() method where all the PushButton actions are defined. Take note of the signature, and use of the word "override"

  void button(int pin, int pressType) override {
    LOG1("Found button press on pin: ");
    LOG1(pin);
    LOG1("  type: ");
    LOG1(pressType == SpanButton::LONG ? "LONG" : (pressType == SpanButton::SINGLE) ? "SINGLE"
                                                                                    : "DOUBLE");
    LOG1("\n");

    if (pressType == SpanButton::SINGLE) {
      if (power->getVal()) {
        power->setVal(false);
        digitalWrite(ledPin, LOW);  // set power to false
      } else {
        power->setVal(true);
        digitalWrite(ledPin, HIGH);  // set power to true
      }
    }
  }
};
