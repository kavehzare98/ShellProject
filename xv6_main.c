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

// Function prototypes
void getInput();
void parseInput(Command *cmd);
void printArgs(Command *cmd);
int isAlpha(char ch);
int isNum(char ch);
int isAlphaNum(char ch);
int isChar(char ch);
int parsePrefix(const char *arg, int i);
int parseIdentif(const char *arg, int i);

// define: main()
int main(void) {
  while (1) {
    Command cmd = {0};

    getInput();
    parseInput(&cmd);
    printArgs(&cmd);

    int validPrefix = parsePrefix(cmd.argv[0], 0);
    if (validPrefix >= 0) {
      int validIdentif = parseIdentif(cmd.argv[0], validPrefix);
      printf("validPrefix: %d\nvalidIdentif: %d\n", validPrefix, validIdentif);
    } else {
      printf("validPrefix: %d\n", validPrefix);
    }
  }

  return 0;
} // end of main

// define: getInput()
void getInput() {

  printf("cs143a$ ");
  if (gets(buffer, BUFF_MAX) == 0) {
    printf("ERROR: EOF!");
  }
} // end of getInput()

// define: parseInput()
void parseInput(Command *cmd) {

  int i = 0;
  cmd->argc = 0;

  while (buffer[i] == ' ' && i < BUFF_MAX) {
    i += 1;
  }

  while (buffer[i] != '\n' && i < BUFF_MAX && cmd->argc < MAX_ARGS) {

    if (buffer[i] != '\n') {
      cmd->argv[cmd->argc] = &buffer[i];
      cmd->argc += 1;
    }

    while (buffer[i] != ' ' && i < BUFF_MAX && buffer[i] != '\n') {
      i++;
    }

    while (buffer[i] == ' ' && i < BUFF_MAX && buffer[i] != '\n') {
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

