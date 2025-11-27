// kzare

#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "user/helperFunctions.h"
#include "user/user.h"

#define BUFF_MAX 512
#define MAX_ARGS 32
#define CMD_EXEC 1
#define MAX_CMDS 10

// global variables
char buffer[BUFF_MAX];

typedef struct {
  char *argv[MAX_ARGS];
  int argc;

  // Redirection handling
  int has_in;  // 1 for true and 0 for false
  int has_out; // 1 for true and 0 for false
  char* in_path;
  char* out_path;
} Command;

typedef struct {
  Command cmds[MAX_CMDS];
  int num_cmds;
} Pipeline;

enum builtInCommands { NONE, CD, EXIT, ABOUT };

// Function prototypes
int detectBuiltIns(const char *arg);
void detectRedirection(Command *cmd);
int getInput();
int handleBuiltIn(int type, Command *cmd);
int isIdentif(const char *arg);
int parseIdentif(const char *arg, int i);
int parsePrefix(const char *arg, int i);
void parseInput(Command *cmd);
void printArgs(Command *cmd);
void runCommand(Command *cmd);
int detectPipeline(Command *cmd);
void pipeify(Command *cmd, Pipeline *cmdPipe);

// define: main()
int main(void) {

  while (1) {

    Command cmd = {0};
    Pipeline cmdPipe = {0};
    int inputFlag;
    int builtInType;
    int pipeFlag;

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

    pipeFlag = detectPipeline(&cmd);

    if (pipeFlag) {

      builtInType = detectBuiltIns(cmd.argv[0]);

      if (builtInType == CD || builtInType == EXIT) {
        write(2, "error\n", 6);
      } else {
        pipeify(&cmd, &cmdPipe);
        detectRedirection(&(cmdPipe.cmds[0]));
        detectRedirection(&(cmdPipe.cmds[cmdPipe.num_cmds - 1]));
        runPipeline(&cmdPipe);
      }

    } else {
      detectRedirection(&cmd);
      builtInType = detectBuiltIns(cmd.argv[0]);

      if (builtInType != NONE) {
        handleBuiltIn(builtInType, &cmd);
      } else {
        runCommand(&cmd);
      }
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

// define: detectRedirection()
void detectRedirection(Command *cmd) {

  int first = -1;

  for (int i = 0; i < cmd->argc; i++) {
    if (stringCompare(cmd->argv[i], "<") == 0) {
      cmd->has_in = 1;
      cmd->in_path = cmd->argv[i+1];
      if (first == -1)
        first = i;
    } else if (stringCompare(cmd->argv[i], ">") == 0) {
      cmd->has_out = 1;
      cmd->out_path = cmd->argv[i+1];
      if (first == -1)
        first = i;
    }
  }

  if (cmd->has_in || cmd->has_out) {
    for (int i = first; i < cmd->argc; i++) {
      cmd->argv[i] = ((void*)0);
    }
    cmd->argc = first;
  }
} // end of detectRedirection()

// define: handleBuiltIn()
int handleBuiltIn(int type, Command *cmd) {

  switch (type) {

  case CD: {

    if (cmd->argc == 1) {
      chdir("/");
    } else if (chdir(cmd->argv[1]) != 0) {
      int fd = 2;
      int len = 19;
      char msg[] = "built-in cd: error\n";
      write(fd, msg, len);
    }

    break;
  }
  case EXIT: {

    int rawExitStatus;
    int finalExitStatus;

    if (cmd->argc == 1) {
      finalExitStatus = 0;
    } else if (cmd->argv[1][0] == '-' || cmd->argv[1][0] == '+') {

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

  case ABOUT: {

    char msg[] =
        "SmartShell:\n  This is a product of blood, sweat, and tears.\n  Treat "
        "it gently. Don't make grammatical errors.\n  It doesnt' know how to "
        "handle them.\n  Thanks for using SmartShell.\n\nAuthor: Kaveh Zare\n";
    int msgLen = 200;

    if (cmd->has_out) {
      int fd = open(cmd->out_path, O_WRONLY | O_CREATE | O_TRUNC);
      if (fd >= 0) {
        if (write(fd, msg, msgLen) < 1) {
          printf("error from write() built-in redirect\n");
        } else {
          close(fd);
        }
      } else {
        printf("error from open() built-in redirect\n");
      }
    } else {
      printf(msg);
    }
    break;
  }
  }
  return 0;
} // end of handleBuiltIn()

// define: printArgs()
void printArgs(Command *cmd) {
  for (int i = 0; i < cmd->argc; i++)
    printf("Arg: %s\n", cmd->argv[i]);
} // end of printArgs()

// define: runCommand()
// returns 0 on success and 1 on failure
void runCommand(Command *cmd) {

  int maxIdentifLen = 65;
  char fallbackPath[maxIdentifLen];
  fallbackPath[0] = '/';
  stringCopy(fallbackPath + 1, cmd->argv[0]);

  int pid = fork();

  if (pid == 0) {

    if (cmd->has_in && cmd->has_out == 0) {
      close(0);
      int fd_in = open(cmd->in_path, O_RDONLY);
      if (fd_in < 0) {
        int fd = 2;
        char errMsg[] = "error from runCommand() on open()\n";
        int msgLen = 34;
        write(fd, errMsg, msgLen);
        exit(1);
      }
    } else if (cmd->has_out && cmd->has_in == 0) {
      close(1);
      int fd_out = open(cmd->out_path, O_WRONLY | O_CREATE | O_TRUNC);
      if (fd_out < 0) {
        int fd = 2;
        char errMsg[] = "error from runCommand() on open()\n";
        int msgLen = 34;
        write(fd, errMsg, msgLen);
        exit(1);
      }
    } else if (cmd->has_out && cmd->has_in) {
      close(0);
      int fd_in = open(cmd->in_path, O_RDONLY);

      close(1);
      int fd_out = open(cmd->out_path, O_WRONLY | O_CREATE | O_TRUNC);

      if (fd_in < 0 || fd_out < 0) {
        int fd = 2;
        char errMsg[] = "error from runCommand() on open()\n";
        int msgLen = 34;
        write(fd, errMsg, msgLen);
        exit(1);
      }
    }

    exec(cmd->argv[0], cmd->argv);
    if (isIdentif(cmd->argv[0])) {
      exec(fallbackPath, cmd->argv);
    }
    int fd = 2; // stderr
    int msgLen = 17;
    char errMsg[] = "main exec: error\n";
    write(fd, errMsg, msgLen);
    exit(1);
  } else {
    wait(0);
  }
} // end of runCommand()

// define: isIdentif()
int isIdentif(const char *arg) {
  int flag = 1;
  while (*arg != '\0') {
    if (*arg == '/' || *arg == '.') {
      flag = 0;
      break;
    }
    arg++;
  }
  return flag;
} // end of isIdentif()

// define: detectPipeline()
int detectPipeline(Command *cmd) {
  int flag = 0;
  for (int i = 0; i < cmd->argc; i++) {
    if (stringCompare(cmd->argv[i], "|") == 0) {
      flag = 1;
      break;
    }
  }
  return flag;
} // end of detectPipeline()

// definition: pipeify()
void pipeify(Command *cmd, Pipeline *cmdPipe) {
  int i, j, k;
  i = 0;
  k = 0;
  cmdPipe->num_cmds = 0;
  while (i < cmd->argc) {
    Command new_cmd = {0};
    j = 0;
    while (i < cmd->argc && stringCompare(cmd->argv[i], "|") != 0) {
      new_cmd.argv[j] = cmd->argv[i];
      new_cmd.argc += 1;
      j++;
      i++;
    }
    new_cmd.argv[new_cmd.argc] = 0;

    if (i < cmd->argc) {
      i++;
    }

    cmdPipe->cmds[k] = new_cmd;
    cmdPipe->num_cmds += 1;
    k++;
  }
} // end of pipeify()

// definition: runPipeline()
void runPipeline(Pipeline *cmdPipe) {
  int prev_read_fd = -1;
  for (int i = 0; i < (cmdPipe->num_cmds - 1); i++) {
    int pipefd[2];
    pipe(pipefd);
    int pid = fork();

    if (pid == 0) {
      if (i > 0) {
        close(0);
        dup(prev_read_fd);
        close(prev_read_fd);
      } else if (i < (cmdPipe->num_cmds - 1)) {
        close(1);
        dup(pipefd[1]);
        close(pipefd[0]);
        close(pipefd[1]);
      } else if (i == 0) {
        close(0);
        open((cmdPipe->cmds[0])->in_path, O_RDONLY);
      } else if (i == (cmdPipe->num_cmds - 1)) {
        close(1);
        open((cmdPipe->cmds[cmdPipe->num_cmds - 1])->out_path, O_WRONLY | O_CREATE | O_TRUNC);
      }

      runCommand(cmdPipe->cmds[i]);
    } else {
      wait(0);
    }
  }
} // end of runPipeline()
