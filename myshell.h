// kzare
#ifndef MYSHELL_H
#define MYSHELL_H

#define BUFF_MAX 512
#define MAX_ARGS 32
#define CMD_EXEC 1
#define MAX_CMDS 10

char buffer[BUFF_MAX];

typedef struct {
  char *argv[MAX_ARGS];
  int argc;

  // Redirection handling
  int has_in;  // 1 for true and 0 for false
  int has_out; // 1 for true and 0 for false
  char *in_path;
  char *out_path;
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
void runPipeline(Pipeline *cmdPipe);

#endif
