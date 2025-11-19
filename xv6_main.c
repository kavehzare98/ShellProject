// kzare

#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/user.h"

#define BUFF_MAX 512
#define MAX_ARGS 32
#define CMD_EXEC 1

// global variables
char buffer[BUFF_MAX];

typedef struct {
  char* argv[MAX_ARGS];
  int argc;
} Command;

enum builtInCommands {NONE, CD, EXIT, ABOUT};

// Function prototypes
int getInput();
void parseInput(Command *cmd);
void printArgs(Command *cmd);
int isAlpha(char ch);
int isNum(char ch);
int isAlphaNum(char ch);
int isChar(char ch);
int parsePrefix(const char *arg, int i);
int parseIdentif(const char *arg, int i);
void cleanBuffer();
int stringCompare(const char *s1, const char *st2);

// define: main()
int main(void) {
  while (1) {
    Command cmd;
    int inputFlag;

    cleanBuffer();
    inputFlag =getInput();

    if (inputFlag ==  1) {
      printf("\nbye\n");
      exit(0);
    } else if (inputFlag != 0) {
      continue;
    }

    parseInput(&cmd);

    if (cmd.argc == 0) {
      continue;
    }

    // Sample system call code:
    int pid = fork();
    // Child process
    if (pid == 0) {
      // Child
      exec(cmd.argv[0], cmd.argv);
      // If execution fails...
      int fd = 2; // stderr
      int msgLen = 6;
      char errMsg[] = "error\n";
      write(fd, errMsg, msgLen);
      exit(1);
    // Parent process
    } else {
      wait(0);
    }

    // end of sample code

    int validPrefix = parsePrefix(cmd.argv[0], 0);
    if (validPrefix >= 0) {
      parseIdentif(cmd.argv[0], validPrefix);
    } else {
      continue;
    }
  }

  return 0;
} // end of main

void cleanBuffer() {
  for (int i = 0; i < BUFF_MAX; i++)
    buffer[i] = '\0';
}

// define: getInput()
// returns:
//   -1 on EOF
//    0 on success
//    1 on Ctrl+D
//    2 on Space(s) + \n Only
int getInput() {

  int fd = 2;
  int strLen = 8;
  int i = 0;
  int foundNonSpace = 0;
  char buf[] = "cs143a$ ";

  write(fd, buf, strLen);

  gets(buffer, BUFF_MAX);

  // Case: Ctrl + D
  if (buffer[0] == '\0') {
    return 1;
  }

  // Check for non-space character before '\n' or '\0'
  while (i < BUFF_MAX) {
    char c = buffer[i];

    if (c == '\n' || c == '\0')
      break;

    if (c != ' ')
      foundNonSpace = 1;

    i += 1;
  }

  if (!foundNonSpace) {
    // Line is only spaces and/or newline
    return 2;
  }

  // Successful input
  return 0;
} // end of getInput()

// define: parseInput()
void parseInput(Command *cmd) {

  int i = 0;
  cmd->argc = 0;

  while (i < BUFF_MAX && cmd->argc < MAX_ARGS) {
    while (i < BUFF_MAX && buffer[i] == ' ')
      i++;

    if (i >= BUFF_MAX || buffer[i] == '\n' || buffer[i] == '\0')
      break;

    cmd->argv[cmd->argc] = &buffer[i];
    cmd->argc++;

    while (i < BUFF_MAX && buffer[i] != ' ' && buffer[i] != '\n' &&
           buffer[i] != '\0') {
      i++;
    }

    if (i < BUFF_MAX && (buffer[i] == ' ' || buffer[i] == '\n')) {
      buffer[i] = '\0';
      i++;
    }
  }

  cmd->argv[cmd->argc] = 0;
} // end of parseInput()


// define: printArgs()
void printArgs(Command *cmd) {
  for (int i = 0; i < cmd->argc; i++)
    printf("Arg: %s\n", cmd->argv[i]);
} // end of printArgs()

// define: parsePrefix()
//  if no prefix found, return same index as the one passed to function
//  if prefix found, return index after the prefix
int parsePrefix(const char *arg, int i) {

  // No prefix cases (2): null term or lack of '.', '/'
  if (arg[i] == '\0' || (arg[i] != '.' && arg[i] != '/'))
    return i;

  // Prefix case: '/'
  if (arg[i] == '/')
    return i + 1;

  // Prefix case: "./"
  if (arg[i] == '.' && arg[i + 1] == '/')
    return i + 2;

  // Prefix case: "../"
  if (arg[i] == '.' && arg[i + 1] == '.' && arg[i + 2] == '/')
    i += 3;

  // check for repeating "../"
  while (arg[i] == '.' && arg[i + 1] == '.' && arg[i + 2] == '/') {
    // Prefix case: repeating "../"
    i += 3;
  }
  // Returns the index of the element after prefix
  return i;
} // end of parsePrefix()

// define: parseIdentif()
//  if identif is invalid, return -1
//  if no identif found, return same index as the one passed to function
//  if identif found, return index after the prefix
int parseIdentif(const char *arg, int i) {

  if (isAlpha(arg[i]) == 0)
    return -1;

  i++;

  while (arg[i] != '/' && arg[i] != '\0') {
    if (isAlphaNum(arg[i]) == 0)
      return -1;
    i++;
  }

  return i;
} // end of parseIdentif()

// define: isAlpha()
int isAlpha(char ch) {

  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
} // end of isAlpha()

// define: isNum()
int isNum(char ch) { return (ch >= '0' && ch <= '9'); } //  end of isNum()

// define: isAlphaNum()
int isAlphaNum(char ch) {

  return (isAlpha(ch) || isNum(ch));
} // end of isAlphaNum()

// define: isChar()
int isChar(char ch) {

  return (ch != '-' && ch != '_' && ch != '+' && ch != '%' && ch != '@' &&
          ch != '/' && ch != '.' && ch != ',' && isAlphaNum(ch) == 0);
} // end of isChar()

// define: handleBuiltIn()
int handleBuiltIn(int type) {
  switch (type) {
    case CD:
      break;
    case EXIT:
      break;
    case ABOUT:
      break;
  }
} // end of handleBuiltIn()

// define: detectBuiltIns()
int detectBuiltIns(const char* arg) {

  int start = 0;
  start = parsePrefix(arg, start);

  if (stringCompare(arg+start, "cd") == 0) {
    return CD;
  } else if (stringCompare(arg+start, "exit") == 0) {
    return EXIT;
  } else if (stringCompare(arg+start, "about") == 0) {
    return ABOUT;
  }

  return NONE;
} // end of detectBuiltIns()

// define; stringCompare()
int stringCompare(const char *s1, const char *st2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
} // end of stringCompare()
