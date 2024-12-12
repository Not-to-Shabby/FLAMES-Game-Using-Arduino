# FLAMES-Game-Using-Arduino

A fun relationship calculator game implemented on Arduino using a rotary encoder and LCD display. The game calculates the relationship between two names using the classic FLAMES algorithm (Friends, Love, Affection, Marriage, Enemy, Sibling).

## Hardware Requirements

- Arduino board (Uno or compatible)
- 16x2 I2C LCD Display (0x27 address)
- Rotary Encoder with push button
- Jumper wires
- Breadboard

## Pin Connections

- LCD I2C:
  - SDA → Arduino SDA pin
  - SCL → Arduino SCL pin
  - VCC → 5V
  - GND → GND

- Rotary Encoder:
  - CLK → Digital Pin 2
  - DT → Digital Pin 3
  - SW → Digital Pin 4
  - VCC → 5V
  - GND → GND

## Features

- Interactive name input using rotary encoder
- 16x2 LCD display with I2C interface
- Demo mode with auto-play feature
- Two-name input system
- FLAMES relationship calculation
- User-friendly interface with real-time feedback
- Automatic demo mode after 10 seconds of inactivity
- Display rotation support for flexible mounting

## Installation

1. Install required libraries in Arduino IDE:
   - Wire.h (built-in)
   - LiquidCrystal_I2C
   - string.h (built-in)

2. Connect hardware according to pin configuration
3. Upload the provided code to your Arduino

## How to Play

1. The game starts in demo mode, showing example gameplay
2. Interact with the rotary encoder to exit demo mode
3. Enter the first name:
   - Rotate to select letters
   - Short press to confirm each letter
   - Long press (1 second) to finish first name
4. Enter the second name using the same method
5. View the FLAMES calculation result
6. Game automatically resets for the next round

## Controls

- Rotate encoder: Select letters
- Short press: Confirm selected letter
- Long press: Complete name entry
- Auto-timeout: Returns to demo mode after 10 seconds of inactivity

## FLAMES Algorithm

The game calculates relationships based on the following categories:
- F: Friends
- L: Love
- A: Affection
- M: Marriage
- E: Enemy
- S: Sibling

The algorithm:
1. Removes common characters from both names
2. Counts remaining characters
3. Uses count to eliminate letters from "FLAMES"
4. Final remaining letter determines relationship type

## Demo Mode

- Automatically starts after 10 seconds of inactivity
- Shows example gameplay with sample names
- Demonstrates full FLAMES calculation process
- Exit demo mode by interacting with rotary encoder

## Customization Options

You can modify these constants in the code:
```cpp
const unsigned long LONG_PRESS_TIME = 1000;    // Button long press duration
const unsigned long ROTARY_DELAY = 150;        // Encoder sensitivity
const unsigned long DEMO_TIMEOUT = 10000;      // Demo mode timeout
const unsigned long DEMO_CHAR_DELAY = 300;     // Demo animation speed
```

## Troubleshooting

1. LCD not displaying:
   - Check I2C address (default 0x27)
   - Verify I2C connections
   - Ensure LCD backlight is on

2. Rotary encoder issues:
   - Check pin connections
   - Verify pull-up resistors
   - Adjust ROTARY_DELAY if input is too sensitive

3. Demo mode problems:
   - Verify timeout settings
   - Check button connections
   - Ensure proper initialization
