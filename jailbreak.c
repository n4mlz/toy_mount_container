#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PATHNAME_SIZE 512

int main() {
  char pathname[PATHNAME_SIZE];
  memset(pathname, '\0', PATHNAME_SIZE);

  getcwd(pathname, PATHNAME_SIZE);
  fprintf(stdout, "cwd: %s\n", pathname);

  mkdir("test", 0);
  chroot("test");

  getcwd(pathname, PATHNAME_SIZE);
  fprintf(stdout, "cwd: %s\n", pathname);

  chdir("../../../../../../../../");

  getcwd(pathname, PATHNAME_SIZE);
  fprintf(stdout, "cwd: %s\n", pathname);

  sleep(3600);

  return 0;
};
