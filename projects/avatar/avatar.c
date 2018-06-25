#include <stdio.h>
#include <avr/io.h> 
#include <avr/power.h>
#include "USART.h"

#define CMDS "tih"

#define HELP \
  NL \
  "Name: First shell" NL \
  "Version: 0.1" NL \
  NL \
  "Available commands:" NL \
  "    t toggle light" NL \
  "    i print program info" NL \
  "    h print help" NL
  
#include "shell.h"

void initPins(void) {
  DDRD |= (1 << PD6);
}

void toggleLed(void) {
  PORTD ^= (1 << PD6);
}

void printInfo(void) {
  printString("Nothing to show so far");
  printNewLine();
}

void executeCmd(char cmd) {
  if (cmd == 't') toggleLed();
  if (cmd == 'i') printInfo();
  if (cmd == 'h') printHelp();
}

int main(void) {
  initUSART();
  initShell(CMDS, HELP);
  initPins();

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
