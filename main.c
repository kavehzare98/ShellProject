#include <stdio.h>

#define BUFF_MAX 1024
#define PATH_MAX 4096
#define ARG_MAX 256
#define MAX_ARG_LEN 20

// global variables
char buffer[BUFF_MAX];

struct Command {
  // char path[PATH_MAX];
  char argv[ARG_MAX][MAX_ARG_LEN];
  int argc;
};

// Function prototypes
void getInput();
void parseInput(struct Command *cmd);
void printArg(const char *arg);
int isAlpha(char ch);
int isNum(char ch);
int isAlphaNum(char ch);
int parsePrefix(const char *arg, int i);

// define: main()
int main(void) {
  struct Command cmd = {0};

  getInput();
  parseInput(&cmd);

  for (int a = 0; a < cmd.argc; a++) {
    printArg(cmd.argv[a]);
  }

  int validPrefix = parsePrefix(cmd.argv[0], 0);

  if (validPrefix == -1)
    printf("\nInvalid Prefix!\n");
  else if (validPrefix == 0)
    printf("\nNo Prefix!\n");
  else
    printf("Prefix detected and ends at index: %d", validPrefix - 1);

  return 0;
} // end of main

// define: getInput()
void getInput() {
  printf("cs143a$ ");
  if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
    printf("ERROR: EOF!");
  }
} // end of getInput()

// define: parseInput()
void parseInput(struct Command *cmd) {
  int i = 0;
  int j = 0;
  while (buffer[i] != '\n') {
    while (buffer[i] == ' ') {
      if (j != 0) {
        cmd->argv[cmd->argc][j] = '\0';
        j = 0;
        cmd->argc += 1;
      }
      i++;
    }

    if (buffer[i] == '\n' || buffer[i] == '\0') {
      break;
    }

    cmd->argv[cmd->argc][j] = buffer[i];
    j++;
    i++;
  }

  if (j != 0) {
    cmd->argv[cmd->argc][j] = '\0';
    cmd->argc += 1;
  }
} // end of parseInput()

// define: printArg()
void printArg(const char *arg) {
  printf("Arg: ");
  for (int i = 0; i < MAX_ARG_LEN && arg[i] != '\0'; i++) {
    printf("%c", arg[i]);
  }
  printf("\n");
} // end of printArg()

// define: parsePrefix()
//  if prefix is invalid, return -1
//  if no prefix found, return same index as the one passed to function
//  if prefix found, return index after the prefix
int parsePrefix(const char *arg, int i) {
  // Invalid cases (3): '//', '...', './/'
  if ((arg[i] == '/' && arg[i + 1] == '/') ||
      (arg[i] == '.' && arg[i + 1] == '.' && arg[i + 2] != '/') ||
      (arg[i] == '.' && arg[i + 1] == '/' && arg[i + 2] == '/') ||
      (arg[i] == '.' && (arg[i + 1] != '/' && arg[i + 1] != '.')))
    return -1;
  // No prefix case
  else if (arg[i] != '.' && arg[i] != '/')
    return i;
  // Prefix case: '/' + end of prefix
  else if (arg[i] == '/' && arg[i + 1] != '.')
    return i + 1;
  // Prefix case: './'
  else if (arg[i] == '.' && arg[i + 1] == '/' && arg[i + 2] != '.')
    return i + 2;
  // Prefix starts with '/'
  else if (arg[i] == '/')
    i += 1;
  // Prefix starts with './'
  else if (arg[i] == '.' && arg[i + 1] == '/')
    i += 2;
  // Prefix starts with '../'
  else if (arg[i] == '.' && arg[i + 1] == '.' && arg[i + 2] == '/')
    i += 3;
  // Looks for {'../'}
  while (arg[i] == '.' && arg[i + 1] == '.' && arg[i + 2] == '/' &&
         i < MAX_ARG_LEN && (i + 1) < MAX_ARG_LEN && (i + 2) < MAX_ARG_LEN)
    i += 3;
  // Returns the index of the element after prefix
  return i;
}

// int parseIdentif(char *arg, int i) {}

// define: isAlpha()
int isAlpha(char ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
} // end of isAlpha()

// define: isNum()
int isNum(char ch) { return (ch >= '1' && ch <= '9'); } //  end of isNum()

// define: isAlphaNum()
int isAlphaNum(char ch) {
  return (isAlpha(ch) || isNum(ch));
} // end of isAlphaNum()

// define: isChar()
int isChar(char ch) {
  return (ch != '-' && ch != '_' && ch != '+' && ch != '%' && ch != '@' &&
          ch != '/' && ch != '.' && ch != ',' && isAlphaNum(ch) == 0);
} // end of isChar()




