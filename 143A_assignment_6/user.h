// kzare

#define SBRK_ERROR ((char *)-1)

struct stat;

// system calls
char* sys_sbrk(int,int);
int chdir(const char*);
int close(int);
int dup(int);
int exec(const char*, char**);
int exit(int) __attribute__((noreturn));
int fork(void);
int fstat(int fd, struct stat*);
int getpid(void);
int kill(int);
int link(const char*, const char*);
int mkdir(const char*);
int mknod(const char*, short, short);
int open(const char*, int);
int pause(int);
int pipe(int*);
int read(int, void*, int);
int unlink(const char*);
int uptime(void);
int wait(int*);
int write(int, const void*, int);
int ps(int *counters);

// ulib.c
char* gets(char*, int max);
char* sbrk(int);
char* sbrklazy(int);
char* strchr(const char*, char c);
char* strcpy(char*, const char*);
int atoi(const char*);
int memcmp(const void *, const void *, uint);
int stat(const char*, struct stat*);
int strcmp(const char*, const char*);
uint strlen(const char*);
void *memcpy(void *, const void *, uint);
void *memmove(void*, const void*, int);
void* memset(void*, int, uint);

// printf.c
void fprintf(int, const char*, ...) __attribute__ ((format (printf, 2, 3)));
void printf(const char*, ...) __attribute__ ((format (printf, 1, 2)));

// umalloc.c
void* malloc(uint);
void free(void*);
