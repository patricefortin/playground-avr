#define NL "\r\n"

#define MSG_PROMPT "avr> "
#define MSG_GREETINGS NL "---- AVR Shell ----" NL

#define MSG_INVALID_CMD "Invalid cmd. Press 'h' for help." NL

void initShell(const char *cmds, const char *help);
void printGreetings(void);
void printInfo(void);
void printHelp(void);
void printInvalidCmd(void);
void echoCmd(char cmd);
char getCmd(void);
int isValidCmd(char cmd);


