#include <stdio.h>
#include <avr/io.h> 
#include <avr/power.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "shell.h"

#define CMDS "tmjknhldi?"

#define HELP \
  NL \
  "Name: Avatar project" NL \
  "Version: 0.1" NL \
  NL \
  "Available commands:" NL \
  "    t toggle light" NL \
  "    m vertical servo middle" NL \
  "    j vertical servo down" NL \
  "    k vertical servo up" NL \
  "    n horizontal servo middle" NL \
  "    h horizontal servo left" NL \
  "    l horizontal servo right" NL \
  "    d toggle debug" NL \
  "    i print info" NL \
  "    ? print help" NL

#define LED_PIN PD6
#define LED_PORT PORTD
#define LED_DDR DDRD

#define V_SERVO_PIN PB1
#define V_SERVO_DDR DDRB

#define H_SERVO_PIN PB2
#define H_SERVO_DDR DDRB

#define PULSE_FREQUENCY   20000

// Servo model is FS5323M
#define V_PULSE_STEP        50
#define V_PULSE_MIN         1400   // Experimental min from physical setup
#define V_PULSE_MIDDLE      1800
#define V_PULSE_MAX         2280   // Experimental max from physical setup
#define V_PULSE_RANGE       (V_PULSE_MAX - V_PULSE_MIN)

#define H_PULSE_STEP        15
#define H_PULSE_MIN         500  // Absolute min
#define H_PULSE_MIDDLE      1500
#define H_PULSE_MAX         2500 // Absolute max
#define H_PULSE_RANGE       (H_PULSE_MAX - H_PULSE_MIN)


volatile uint8_t currentAngle = 90;
volatile uint8_t debug = 0;

void initPins(void) {
  V_SERVO_DDR |= (1 << V_SERVO_PIN);
  H_SERVO_DDR |= (1 << H_SERVO_PIN);
  LED_DDR |= (1 << LED_PIN);
}

static inline void initTimer1Servo(void) {
  TCCR1A |= (1 << COM1A1);                // Direct output on PB1 / OC1A. As long as OCR1A is not reached, output is high, after output is low
  TCCR1A |= (1 << COM1B1);                // Direct output on PB2 / OC1B. As long as OCR1B is not reached, output is high, after output is low
  TCCR1A |= (1 << WGM11);                 // Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
  TCCR1B |= (1 << WGM12) | (1 << WGM13);  // counter max in ICR1
  TCCR1B |= (1 << CS10);                  // /1 prescaling -- counting in microseconds (fuse CKDIV8 must be programmed, so internal clock is 1MHz)
  ICR1 = PULSE_FREQUENCY;                 // Top value, when it restart counting
}

void toggleLed(void) {
  LED_PORT ^= (1 << LED_PIN);
}

/* Vertical Servo */

void setVServoByAngle(uint8_t angle) {
  uint32_t pulseLen = (uint32_t)angle * V_PULSE_RANGE / 180 + V_PULSE_MIN;
  OCR1A = pulseLen;
  currentAngle = angle;
  if (debug) {
    printString("Vertical pulse length: ");
    printInt32(pulseLen);
  }
}

void resetVServoMiddle(void) {
  setVServoByAngle(90);
}

void moveVServoUp(void) {
  // Physical up is decreasing angle
  uint8_t safe = V_PULSE_STEP;
  uint8_t angle = currentAngle < safe ? 0 : currentAngle - V_PULSE_STEP; // avoid underflow
  setVServoByAngle(angle);
}

void moveVServoDown(void) {
  // Physical down is increasing angle
  uint8_t safe = 180 - V_PULSE_STEP;
  uint8_t angle = currentAngle > safe ? 180 : currentAngle + V_PULSE_STEP;
  setVServoByAngle(angle);
}

/* Horizontal Servo */

void setHServoByAngle(uint8_t angle) {
  uint32_t pulseLen = (uint32_t)angle * H_PULSE_RANGE / 180 + H_PULSE_MIN;
  OCR1B = pulseLen;
  currentAngle = angle;
  if (debug) {
    printString("Horizontal pulse length: ");
    printInt32(pulseLen);
  }
}

void resetHServoMiddle(void) {
  setHServoByAngle(90);
}

void moveHServoLeft(void) {
  // Physical left is increase angle
  uint8_t safe = 180 - H_PULSE_STEP;
  uint8_t angle = currentAngle > safe ? 180 : currentAngle + H_PULSE_STEP;
  setHServoByAngle(angle);
}

void moveHServoRight(void) {
  // Physical right is decrease angle
  uint8_t safe = H_PULSE_STEP;
  uint8_t angle = currentAngle < safe ? 0 : currentAngle - H_PULSE_STEP; // avoid underflow
  setHServoByAngle(angle);
}

void toggleDebug(void) {
  debug = debug ? 0 : 1;
}

void printInfo(void) {
  printString("Angle: ");
  printByte(currentAngle);
  
  printString("Vertical pulse length: ");
  printWord(OCR1A);
  
  printString("Horizontal pulse length: ");
  printWord(OCR1B);
  
  printString("Debug: ");
  printByte(debug);
}

void executeCmd(char cmd) {
  if (cmd == 't') toggleLed();

  if (cmd == 'm') resetVServoMiddle();
  if (cmd == 'j') moveVServoDown();
  if (cmd == 'k') moveVServoUp();
  
  if (cmd == 'n') resetHServoMiddle();
  if (cmd == 'h') moveHServoLeft();
  if (cmd == 'l') moveHServoRight();

  if (cmd == 'd') toggleDebug();
  if (cmd == 'i') printInfo();
  if (cmd == '?') printHelp();
}

int main(void) {
  initUSART();
  initShell(CMDS, HELP);
  initPins();
  initTimer1Servo();

  sei(); // set enable interrupt bit

  printGreetings();

  while (1) {
    char cmd = getCmd();
    echoCmd(cmd);

    if (isValidCmd(cmd)) {
      executeCmd(cmd);
    } else {
      printInvalidCmd();
    }
  }
  return 0;
}
