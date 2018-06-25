#include <avr/io.h> 
#include <string.h> 
#include "shell.h"
#include "USART.h"

const char *globalCmds;
const char *globalHelp;

void initShell(const char *cmds, const char *help) {
  globalCmds = cmds;
  globalHelp = help;
}

void printGreetings(void) {
  printString(MSG_GREETINGS);
}

void printHelp(void) {
  printString(globalHelp);
}

void printInvalidCmd(void) {
  printString(MSG_INVALID_CMD);
}

void echoCmd(char cmd) {
  printChar(cmd);
  printNewLine();
}

char getCmd(void) {
  printString(MSG_PROMPT);
  return receiveByte();
}

int isValidCmd(char cmd) {
  int i;
  int len = strlen(globalCmds);
  for (i = 0; i < len; i++) {
    if (globalCmds[i] == cmd) {
      return 1;
    }
  }
  return 0;
}


