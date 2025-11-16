#include <stdio.h>

#define BUFF_MAX 1024
#define PATH_MAX 4096
#define ARG_MAX 256
#define MAX_ARG_LEN 20

struct Command {
  //char path[PATH_MAX];
  char argv[ARG_MAX][MAX_ARG_LEN];
  int argc;
};

void printArgument(char arg[], int size);

int main(void)
{
  int i = 0;
  int j = 0;
  struct Command cmd = { 0 };
  char buffer[BUFF_MAX];

  printf("cs143a$ ");
  if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
    printf("ERROR: EOF!");
  }

  while (buffer[i] != '\n') {
    while (buffer[i] == ' ') {
      if (j != 0) {
        cmd.argv[cmd.argc][j] = '\0';
        j = 0;
        cmd.argc += 1;
      }
      i++;
    }

    if (buffer[i] == '\n' || buffer[i] == '\0') {
      break;
    }

    cmd.argv[cmd.argc][j] = buffer[i];
    j++;
    i++;
  }

  if (j != 0) {
    cmd.argv[cmd.argc][j] = '\0';
    cmd.argc += 1;
  }

  printf("argc: %d\n", cmd.argc);
  for (int a = 0; a < cmd.argc; a++) {
    printArgument(cmd.argv[a], MAX_ARG_LEN);
  }

  printf("\nDone!\n");
  return 0;
}



void printArgument(char arg[], int size) {
  printf("Arg: ");
  for (int i = 0; i < size && arg[i] != '\0'; i++) {
    printf("%c", arg[i]);
  }
  printf("\n");
}
