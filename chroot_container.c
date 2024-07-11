#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define NEW_ROOT "./chroot_fs"

int main() {
  mkdir(NEW_ROOT, 0755);

  system("cp -r ./image/alpine/* " NEW_ROOT "/");

  if (unshare(CLONE_NEWNS) == -1) {
    perror("unshare");
    return 1;
  }

  mount("none", "/", NULL, MS_REC | MS_PRIVATE, NULL);

  mount(NEW_ROOT, NEW_ROOT, NULL, MS_BIND, NULL);
  mount("proc", NEW_ROOT "/proc", "proc", 0, NULL);

  if (chroot(NEW_ROOT) == -1) {
    perror("chroot");
    return 1;
  }

  if (chdir("/") == -1) {
    perror("chdir");
    return 1;
  }

  execlp("/bin/sh", "sh", NULL);

  return 0;
}
