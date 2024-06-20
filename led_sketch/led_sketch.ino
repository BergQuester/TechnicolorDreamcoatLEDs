char currentMode = '0';

void setup() {
  Serial.begin(9600);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  while(!Serial) {
    ; // Wait
  }

  Serial.println("Connected");
}

// the loop function runs over and over again forever
void loop() {
  // if (currentMode != '1') {
  //   return;
  // }

  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  // Serial.println("On");
  if (delayWhileInMode(3000, '1')) { 
    digitalWrite(LED_BUILTIN, LOW);
    return;
  }                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  // Serial.println("Off");
  if (delayWhileInMode(3000, '1')) { return; }                      // wait for a second
}

// Creates a dealy while in a mode
// Returns true if the dealy was canceled by a mode change
bool delayWhileInMode(unsigned long delayTime, char mode) {

  unsigned long remainingDelay = delayTime;
  unsigned long checkInterval = 20;
  do {
    if (currentMode != mode) { return true; }
    if (remainingDelay < checkInterval) {
      checkInterval = remainingDelay;
      remainingDelay = 0;
    } else {
      remainingDelay -= checkInterval;
    }

    delay(checkInterval);
  } while (remainingDelay == 0);

  return false;
}

void serialEvent() {
    if (Serial.available()) {
      // get the new byte:
      char inputChar = (char)Serial.read();
      if (inputChar == '\n') {  // don't care about newlines
        return;
      }
      currentMode = inputChar;
      Serial.println(currentMode);

      // clear buffer (only interested in the first character)
      while (Serial.available()) {
        char devNull = (char)Serial.read();
      }
    }
}