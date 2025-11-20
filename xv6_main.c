// kzare

#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/helperFunctions.h"
#include "user/user.h"

#define BUFF_MAX 512
#define MAX_ARGS 32
#define CMD_EXEC 1

// global variables
char buffer[BUFF_MAX];

typedef struct {
  char *argv[MAX_ARGS];
  int argc;
} Command;

enum builtInCommands { NONE, CD, EXIT, ABOUT };

// Function prototypes
int detectBuiltIns(const char *arg);
int getInput();
int handleBuiltIn(int type, Command *cmd);
int parseIdentif(const char *arg, int i);
int parsePrefix(const char *arg, int i);
void parseInput(Command *cmd);
void printArgs(Command *cmd);

// define: main()
int main(void) {

  while (1) {
    Command cmd;
    int inputFlag;

    inputFlag = getInput();

    if (inputFlag == 1) {
      printf("\nbye\n");
      exit(0);
    } else if (inputFlag != 0) {
      continue;
    }

    parseInput(&cmd);

    if (cmd.argc == 0) {
      continue;
    }

    int builtInType = detectBuiltIns(cmd.argv[0]);
    if (builtInType != NONE) {
      handleBuiltIn(builtInType, cmd.argv[1], &cmd);
    }

    int pid = fork();
    if (pid == 0) {
      exec(cmd.argv[0], cmd.argv);
      int fd = 2; // stderr
      int msgLen = 6;
      char errMsg[] = "error\n";
      write(fd, errMsg, msgLen);
      exit(1);
    } else {
      wait(0);
    }

    int validPrefix = parsePrefix(cmd.argv[0], 0);
    if (validPrefix >= 0) {
      parseIdentif(cmd.argv[0], validPrefix);
    } else {
      continue;
    }
  }

  return 0;
} // end of main

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
    return 2;
  }

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

// define: handleBuiltIn()
int handleBuiltIn(int type, Command *cmd) {

  switch (type) {

  case CD: {

    if (chdir(cmd->argv[1]) != 0) {
      printf("error\n");
    }
    break;
  }
  case EXIT: {

    int rawExitStatus;
    int finalExitStatus;

    if (cmd->argc == 1) {
      finalExitStatus = 0;
    } else if (cmd->argv[1][0] == '-' || arg[0] == '+') {

      if (isNum(cmd->argv[1][1]) == 1) {
        rawExitStatus = stringToInteger(cmd->argv[1]);
        if (rawExitStatus >= -128 && rawExitStatus <= 128) {
          finalExitStatus = rawExitStatus;
        } else {
          finalExitStatus = -1;
        }
      } else {
        finalExitStatus = -1;
      }
    } else if (isNum(cmd->argv[1][0]) == 1) {
      rawExitStatus = stringToInteger(cmd->argv[1]);
      if (rawExitStatus >= -128 && rawExitStatus <= 128) {
        finalExitStatus = rawExitStatus;
      } else {
        finalExitStatus = -1;
      }
    } else {
      finalExitStatus = -1;
    }
    printf("bye\n");
    exit(finalExitStatus);
    break;
  }

  case ABOUT:
    printf("ABOUT command detected and handled!\n");
    break;
  }
  return 0;
} // end of handleBuiltIn()

// define: detectBuiltIns()
int detectBuiltIns(const char *arg) {

  if (stringCompare(arg, "cd") == 0) {
    return CD;
  } else if (stringCompare(arg, "exit") == 0) {
    return EXIT;
  } else if (stringCompare(arg, "about") == 0) {
    return ABOUT;
  }

  return NONE;
} // end of detectBuiltIns()

// define: printArgs()
void printArgs(Command *cmd) {
  for (int i = 0; i < cmd->argc; i++)
    printf("Arg: %s\n", cmd->argv[i]);
} // end of printArgs()

