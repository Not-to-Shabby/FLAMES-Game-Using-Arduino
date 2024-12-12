#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Rotary Encoder pins
#define CLK 2
#define DT 3
#define SW 4

// Game variables
char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int currentPos = 0;
char name1[16] = "";
char name2[16] = "";
int nameIndex = 0;
bool isFirstName = true;
int lastButtonState = HIGH;
int lastCLK;

// Add these global variables
unsigned long buttonPressStartTime = 0;
const unsigned long LONG_PRESS_TIME = 1000; // 1 second
bool buttonActive = false;
enum GameState {NAME1, NAME2, RESULT};
GameState currentState = NAME1;

// Add these global variables at the top
unsigned long lastRotaryUpdate = 0;
const unsigned long ROTARY_DELAY = 150; // Adjust this value to control speed (higher = slower)

// Add these global variables
bool inDemoMode = true;
unsigned long lastDemoUpdate = 0;
const unsigned long DEMO_CHAR_DELAY = 300;
const char* DEMO_NAME1 = "TANEX";
const char* DEMO_NAME2 = "ELIZABETH";
int demoState = 0;
int demoCharIndex = 0;

// Add these global variables
unsigned long lastUserAction = 0;
const unsigned long DEMO_TIMEOUT = 10000; // 10 seconds

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setRotateDisplay(true);  // Rotate display 180 degrees
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  lastCLK = digitalRead(CLK);
  
  lcd.print("     FLAMES     ");
  delay(2000);
  lcd.clear();
  lcd.print("1ST NAME:");
}

// Modify loop() function
void loop() {
  if (inDemoMode) {
    if (digitalRead(CLK) != lastCLK || digitalRead(SW) == LOW) {
      inDemoMode = false;
      lastUserAction = millis();
      lcd.clear();
      lcd.print("1ST NAME:");
      lcd.setCursor(0, 1);
      return;
    }
    playDemoAnimation();
  } else {
    // Check for timeout
    if (millis() - lastUserAction >= DEMO_TIMEOUT) {
      inDemoMode = true;
      demoState = 0;
      demoCharIndex = 0;
      lcd.clear();
      lcd.print("1ST NAME:");
      return;
    }
    handleRotaryInput();
    handleButtonPress();
  }
}

// Modify handleRotaryInput() function
void handleRotaryInput() {
  int clkState = digitalRead(CLK);
  
  if (clkState != lastCLK && (millis() - lastRotaryUpdate) > ROTARY_DELAY) {
    lastUserAction = millis(); // Update on interaction
    if (digitalRead(DT) != clkState) {
      currentPos++;
      if (currentPos >= strlen(alphabet)) currentPos = 0;
    } else {
      currentPos--;
      if (currentPos < 0) currentPos = strlen(alphabet) - 1;
    }
    lcd.setCursor(nameIndex, 1);
    lcd.print(alphabet[currentPos]);
    
    lastRotaryUpdate = millis(); // Update timing
  }
  lastCLK = clkState;
}

// Replace handleButtonPress() function
void handleButtonPress() {
  int buttonState = digitalRead(SW);
  
  if (buttonState != lastButtonState) {
    lastUserAction = millis(); // Update on interaction
  }
  
  // Button just pressed
  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressStartTime = millis();
    buttonActive = true;
  }
  
  // Button is being held
  if (buttonState == LOW && buttonActive) {
    // Check for long press
    if ((millis() - buttonPressStartTime) >= LONG_PRESS_TIME) {
      switch(currentState) {
        case NAME1:
          // Switch to name 2
          currentState = NAME2;
          name1[nameIndex] = '\0'; // Terminate first name
          nameIndex = 0;
          lcd.clear();
          lcd.print("2ND NAME:");
          lcd.setCursor(0, 1);
          lcd.blink();
          delay(200);
          lcd.noBlink();
          break;
          
        case NAME2:
          // Calculate result
          currentState = RESULT;
          name2[nameIndex] = '\0'; // Terminate second name
          calculateFlames();
          break;
      }
      buttonActive = false;
    }
  }
  
  // Button released (short press)
  if (buttonState == HIGH && lastButtonState == LOW) {
    if (buttonActive && (millis() - buttonPressStartTime) < LONG_PRESS_TIME) {
      // Handle regular character input
      switch(currentState) {
        case NAME1:
          name1[nameIndex] = alphabet[currentPos];
          name1[nameIndex + 1] = '\0';
          break;
        case NAME2:
          name2[nameIndex] = alphabet[currentPos];
          name2[nameIndex + 1] = '\0';
          break;
      }
      
      if (nameIndex < 15) {
        nameIndex++;
        lcd.setCursor(nameIndex, 1);
      }
    }
    buttonActive = false;
  }
  
  lastButtonState = buttonState;
}

void calculateFlames() {
  lcd.clear();
  
  // Display both names first
  lcd.print("Name 1:");
  lcd.print(name1);
  lcd.setCursor(0, 1);
  lcd.print("Name 2:");
  lcd.print(name2);
  delay(2000);
  
  // Convert names to lowercase for calculation
  toLowerCase(name1);
  toLowerCase(name2);
  
  // Count remaining characters after removing common ones
  int count = getRemainingCount();
  
  // Calculate FLAMES result
  char result = getFlamesResult(count);
  
  // Clear and show result
  lcd.clear();
  lcd.print("RESULT:");
  lcd.setCursor(0, 1);
  
  // Display result in uppercase
  switch(result) {
    case 'F': lcd.print("FRIENDS"); break;
    case 'L': lcd.print("LOVE"); break;
    case 'A': lcd.print("AFFECTION"); break;
    case 'M': lcd.print("MARRIAGE"); break;
    case 'E': lcd.print("ENEMY"); break;
    case 'S': lcd.print("SIBLING"); break;
  }
  
  delay(5000);
  resetGame();
}

int getRemainingCount() {
  int count = 0;
  bool used1[16] = {0};
  bool used2[16] = {0};
  
  // Mark common characters
  for(int i = 0; name1[i]; i++) {
    for(int j = 0; name2[j]; j++) {
      if(!used2[j] && name1[i] == name2[j]) {
        used1[i] = used2[j] = true;
        break;
      }
    }
  }
  
  // Count unmarked characters
  for(int i = 0; name1[i]; i++) if(!used1[i]) count++;
  for(int i = 0; name2[i]; i++) if(!used2[i]) count++;
  
  return count;
}

char getFlamesResult(int count) {
  char flames[] = "FLAMES";
  int len = 6;
  int pos = 0;
  
  for(int i = 6; i > 1; i--) {
    pos = ((count % i) + pos - 1 + i) % i;
    // Remove character at pos
    for(int j = pos; j < i-1; j++) {
      flames[j] = flames[j+1];
    }
  }
  
  return flames[0];
}

void toLowerCase(char* str) {
  for(int i = 0; str[i]; i++) {
    if(str[i] >= 'A' && str[i] <= 'Z') {
      str[i] += 32;
    }
  }
}

// Modify resetGame() function
void resetGame() {
  currentState = NAME1;
  nameIndex = 0;
  currentPos = 0;
  memset(name1, 0, sizeof(name1));
  memset(name2, 0, sizeof(name2));
  lcd.clear();
  lcd.print("     FLAMES     ");
  delay(3000);
  lcd.clear();
  lcd.print("1ST NAME:");
  lcd.setCursor(0, 1);
}

// Modify playDemoAnimation()
void playDemoAnimation() {
  if (millis() - lastDemoUpdate < DEMO_CHAR_DELAY) {
    return;
  }
  lastDemoUpdate = millis();

  switch(demoState) {
    case 0: // Show FLAMES title
      lcd.clear();
      lcd.print("     FLAMES     ");
      delay(2000);
      lcd.clear();
      lcd.print("1ST NAME:");
      demoState = 1;
      break;

    case 1: // Show first name typing
      if (demoCharIndex < strlen(DEMO_NAME1)) {
        lcd.setCursor(demoCharIndex, 1);
        lcd.print(DEMO_NAME1[demoCharIndex]);
        demoCharIndex++;
      } else {
        demoState = 2;
        demoCharIndex = 0;
        delay(1000);
        lcd.clear();
        lcd.print("2ND NAME:");
      }
      break;

    case 2: // Show second name typing
      if (demoCharIndex < strlen(DEMO_NAME2)) {
        lcd.setCursor(demoCharIndex, 1);
        lcd.print(DEMO_NAME2[demoCharIndex]);
        demoCharIndex++;
      } else {
        demoState = 3;
        delay(1000);
      }
      break;

    case 3: // Show FLAMES calculation
      lcd.clear();
      lcd.print(DEMO_NAME1);
      lcd.setCursor(0, 1);
      lcd.print(DEMO_NAME2);
      delay(2000);
      
      // Show FLAMES elimination
      lcd.clear();
      lcd.print("FLAMES...");
      delay(1000);
      lcd.print("F");
      delay(500);
      lcd.print("L");
      delay(500);
      lcd.print("A");
      delay(500);
      lcd.print("M");
      delay(500);
      lcd.print("E");
      delay(500);
      lcd.print("S");
      delay(1000);
      
      demoState = 4;
      break;

    case 4: // Show result
      lcd.clear();
      lcd.print("Result:");
      lcd.setCursor(0, 1);
      lcd.print("LOVE");
      delay(3000);
      
      // Reset demo
      demoState = 0;
      demoCharIndex = 0;
      break;
  }
}
