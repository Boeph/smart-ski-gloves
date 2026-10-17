//  piezo fall sensor @ a0
//  demo fsr for play/pause music stuff @ a1
//  volume up and down @ d2 and d3
//  siri @ d4

#include <Keyboard.h>

//pinouts
const int PIEZO_PIN = A0;
const int FSR_PIN = A1;
const int VOL_UP_PIN = 2;
const int VOL_DOWN_PIN = 3;
const int SIRI_PIN = 4;

const int FALL_THRESHOLD = 500;
const unsigned long PIEZO_DEBOUNCE = 3000;  

//fsr thresholds below found through testing
const float FSR_MIN_VOLTAGE = 0.02;
const float ACTIVATE_GRAMS = 200.0;
const unsigned long FSR_DEBOUNCE = 60;


//buttons
struct Button {
  const int pin;
  bool lastReading;
  bool state;
  bool held;
  bool singlePress;
  unsigned long pressedAt;
  unsigned long lastRepeatAt;
  unsigned long debounceAt;
};

Button volUp = { VOL_UP_PIN, HIGH, HIGH, false, false, 0, 0, 0 };
Button volDown = { VOL_DOWN_PIN, HIGH, HIGH, false, false, 0, 0, 0 };
Button siriButton = { SIRI_PIN, HIGH, HIGH, false, false, 0, 0, 0 };

//fsr
bool fsrPressed = false;
int pressCount = 0;
unsigned long lastPressTime = 0;
unsigned long pressStartTime = 0;

//piezo
unsigned long lastFall = 0;


void setup() {
  Serial.begin(9600);
  Keyboard.begin();

  pinMode(VOL_UP_PIN, INPUT_PULLUP);
  pinMode(VOL_DOWN_PIN, INPUT_PULLUP);
  pinMode(SIRI_PIN, INPUT_PULLUP);
  
}


void loop() {
  readPiezo();
  readFSR();
  checkMultiPress();
  updateButton(volUp, false);
  updateButton(volDown, false);
  updateButton(siriButton, true);
  delay(20);
}


void readPiezo() {
  int val = analogRead(PIEZO_PIN);
  if (val > FALL_THRESHOLD && millis() - lastFall > PIEZO_DEBOUNCE) {
    lastFall = millis();

    
    //debug Serial.print(val);
    
    triggerSOS();
  }
}

void triggerSOS() {
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_ALT); 
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_GUI);    
  Keyboard.press('s');
  delay(100);
  Keyboard.releaseAll();
}

//fsr: convert reading to grams (10k pulldown, 5V)
void readFSR() {
  int raw = analogRead(FSR_PIN);
  float voltage = raw * (5.0 / 1023.0);
  float grams = 0;

  if (voltage >= FSR_MIN_VOLTAGE) {
    float fsrOhms = ((5.0 - voltage) / voltage) * 10000; //10000 pulldown, 5.0 vcc
    grams = constrain(1000000.0 / fsrOhms, 0, 2000.0); //2kg max load
  }

  bool isPressed = (grams >= ACTIVATE_GRAMS);

  if (isPressed && !fsrPressed) {
    if (millis() - pressStartTime > FSR_DEBOUNCE) {
      fsrPressed = true;
      pressCount++;
      lastPressTime = millis();
    
      //debug Serial.print(pressCount);
      //Serial.print(grams, 0);

    }
  } else if (!isPressed && fsrPressed) {
    fsrPressed = false;
    pressStartTime = millis();
  }
}

void checkMultiPress() {
  if (pressCount == 0 || fsrPressed) return;
  if (millis() - lastPressTime < 500) return;

  
  switch (pressCount) {
    case 1:
      Keyboard.press(KEY_F8);//play/pause
      delay(50);
      Keyboard.releaseAll();
      break;
    case 2:
      Keyboard.press(KEY_F9);//next
      delay(50);
      Keyboard.releaseAll();
      break;
    case 3:
      Keyboard.press(KEY_F7);//prev
      delay(50);
      Keyboard.releaseAll();
      break;
    default:
      break;
  }
  pressCount = 0;
}


//buttons
void updateButton(Button &btn, bool isSiri) {
  bool reading = digitalRead(btn.pin);
  if (reading != btn.lastReading) btn.debounceAt = millis();
  btn.lastReading = reading;
  if (millis() - btn.debounceAt < 50) return;

  bool isDown = (reading == LOW);

  if (isDown && btn.state == HIGH) {
    btn.state = LOW;
    btn.pressedAt = millis();
    btn.held = false;
    btn.singlePress = false;
    btn.lastRepeatAt = millis();
  }

  // Held down
  if (isDown && btn.state == LOW) {
    unsigned long heldFor = millis() - btn.pressedAt;

    if (isSiri) {
      // siri hold for 800 ms
      if (!btn.held && heldFor >= 800) {
        btn.held = true;
        //siri activation
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press(KEY_LEFT_ALT);
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press('s');
        delay(100);
        Keyboard.releaseAll();
      }
    } else {
      // vol, hold for 500 to do ticks at 200ms
      if (!btn.held && heldFor >= 500) {
        btn.held = true;
        btn.singlePress = true;
        activateVolume(btn.pin);
        btn.lastRepeatAt = millis();
      }
      if (btn.held && millis() - btn.lastRepeatAt >= 200) {
        btn.lastRepeatAt = millis();
        activateVolume(btn.pin);
      }
    }
  }

  
  if (!isDown && btn.state == LOW) {
    btn.state = HIGH;
    if (!isSiri && !btn.held && !btn.singlePress) activateVolume(btn.pin);
    btn.held = false;
    btn.singlePress = false;
  }
}

void activateVolume(int pin) {
  if (pin == VOL_UP_PIN) {
    //vol up
    Keyboard.press(KEY_F12);
    delay(50);
    Keyboard.releaseAll();
  }
  if (pin == VOL_DOWN_PIN) {
    //vol down
    Keyboard.press(KEY_F11);
    delay(50);
    Keyboard.releaseAll();
  }
}
