#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>

#define NEW_ROOT "./chroot_fs"

int main() {
    // ディレクトリを作成
    mkdir(NEW_ROOT, 0755);
    
    // Alpine Linux イメージをコピー
    system("cp -r ./image/alpine/* " NEW_ROOT "/");

    // unshare システムコールでプロセスを分離
    if (unshare(CLONE_NEWNS) == -1) {
        perror("unshare");
        return 1;
    }

    // 名前空間を分けたプロセスで bind mount を行う前に shared を外す
    mount("none", "/", NULL, MS_REC|MS_PRIVATE, NULL);

    // マウントポイントを作成
    mount(NEW_ROOT, NEW_ROOT, NULL, MS_BIND, NULL);
    mount("proc", NEW_ROOT "/proc", "proc", 0, NULL);

    // chroot で root を変更
    if (chroot(NEW_ROOT) == -1) {
        perror("chroot");
        return 1;
    }

    // カレントディレクトリを変更
    if (chdir("/") == -1) {
        perror("chdir");
        return 1;
    }

    // シェルを起動
    execlp("/bin/sh", "sh", NULL);

    return 0;
}
