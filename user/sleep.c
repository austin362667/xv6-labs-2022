#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  
  char s[] = "(nothing happens for a little while)\n";
  write(1, s, strlen(s));
  sleep(atoi(argv[1]));
  
  exit(0);
}
