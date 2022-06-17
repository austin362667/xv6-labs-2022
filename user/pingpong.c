#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int p1[2], p2[2];
  pipe(p1);// A pipe of parent process write while child process read. (p1[1]=>p2[0])
  pipe(p2);// A pipe of child process write while parent process read. (p1[0]<=p2[1])
  char buf[64];

  if (fork()) {
    // Parent, when fork() > 0
    write(p1[1], "ping", strlen("ping"));
    read(p2[0], buf, 4);
    printf("%d: received %s\n", getpid(), buf);
  } else {
    // Child, when fork() <=0
    read(p1[0], buf, 4);
    printf("%d: received %s\n", getpid(), buf);
    write(p2[1], "pong", strlen("pong"));
  }

  exit(0);
}
