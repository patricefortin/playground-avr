#include <stdio.h>
#include <avr/io.h> 
#include <avr/power.h>
#include <avr/interrupt.h>
#include "USART.h"
#include "shell.h"

#define CMDS "tmjkdi?"

#define HELP \
  NL \
  "Name: Avatar project" NL \
  "Version: 0.1" NL \
  NL \
  "Available commands:" NL \
  "    t toggle light" NL \
  "    m servo 1 middle" NL \
  "    j servo 1 down" NL \
  "    k servo 1 up" NL \
  "    d toggle debug" NL \
  "    i print info" NL \
  "    ? print help" NL

#define SERVO_PIN PB1
#define SERVO_PORT PORTB
#define SERVO_DDR DDRB

#define LED_PIN PD6
#define LED_PORT PORTD
#define LED_DDR DDRD

// Servo model is FS5323M
#define PULSE_STEP        5
//#define PULSE_MIN         500  // Absolute min
#define PULSE_MIN         1400   // Experimental min from physical setup
#define PULSE_MIDDLE      1800
//#define PULSE_MAX         2500 // Absolute max
#define PULSE_MAX         2280   // Experimental max from physical setup
#define PULSE_RANGE       (PULSE_MAX - PULSE_MIN)
#define PULSE_FREQUENCY   20000

volatile uint8_t currentAngle = 90;
volatile uint8_t debug = 0;

void initPins(void) {
  SERVO_DDR |= (1 << SERVO_PIN);
  LED_DDR |= (1 << LED_PIN);
}

static inline void initTimer1Servo(void) {
  TCCR1A |= (1 << COM1A1);                // Direct output on PB1 / OC1A. As long as OCR1A is not reach, output is high, after output is low
  TCCR1A |= (1 << WGM11);                 // Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
  TCCR1B |= (1 << WGM12) | (1 << WGM13);  // counter max in ICR1
  TCCR1B |= (1 << CS10);                  // /1 prescaling -- counting in microseconds
  ICR1 = PULSE_FREQUENCY;                 // Top value, when it restart counting
}

void toggleLed(void) {
  LED_PORT ^= (1 << LED_PIN);
}

void setServoByAngle(uint8_t angle) {
  uint32_t pulseLen = (uint32_t)angle * PULSE_RANGE / 180 + PULSE_MIN;
  OCR1A = pulseLen;
  currentAngle = angle;
  if (debug) {
    printString("Pulse length: ");
    printInt32(pulseLen);
  }
}

void resetServoMiddle(void) {
  setServoByAngle(90);
}

/*
 * Physical up direction is decreasing angle
 */
void moveServoUp(void) {
  uint8_t safe = PULSE_STEP;
  uint8_t angle = currentAngle < safe ? 0 : currentAngle - PULSE_STEP; // avoid underflow
  setServoByAngle(angle);
}

/*
 * Physical down is increasing angle
 */
void moveServoDown(void) {
  uint8_t safe = 180 - PULSE_STEP;
  uint8_t angle = currentAngle > safe ? 180 : currentAngle + PULSE_STEP;
  setServoByAngle(angle);
}

void toggleDebug(void) {
  debug = debug ? 0 : 1;
}

void printInfo(void) {
  printString("Angle: ");
  printByte(currentAngle);
  
  printString("Pulse length: ");
  printWord(OCR1A);
  
  printString("Debug: ");
  printByte(debug);
}

void executeCmd(char cmd) {
  if (cmd == 't') toggleLed();
  if (cmd == 'm') resetServoMiddle();
  if (cmd == 'j') moveServoDown();
  if (cmd == 'k') moveServoUp();
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
