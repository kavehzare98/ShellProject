#include <stdio.h>

#define BUFF_MAX 1024
#define PATH_MAX 4096
#define ARG_MAX 256
#define MAX_ARG_LEN 20

// global variables
char buffer[BUFF_MAX];

struct Command {
  //char path[PATH_MAX];
  char argv[ARG_MAX][MAX_ARG_LEN];
  int argc;
};

// Function prototypes
void getInput();
void parseInput(struct Command *cmd);
void printArg(char arg[], int size);

// define: main()
int main(void)
{
  struct Command cmd = { 0 };

  getInput();
  parseInput(&cmd);

  for (int a = 0; a < cmd.argc; a++) {
    printArg(cmd.argv[a], MAX_ARG_LEN);
  }

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
void parseInput(struct Command *cmd)
{
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
void printArg(char arg[], int size)
{
  printf("Arg: ");
  for (int i = 0; i < size && arg[i] != '\0'; i++) {
    printf("%c", arg[i]);
  }
  printf("\n");
} // end of printArg()
