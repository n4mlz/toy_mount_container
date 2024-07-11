#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>

#define NEW_ROOT "./pivot_root_fs"

int main() {
    // ディレクトリを作成
    mkdir(NEW_ROOT, 0755);
    
    // Alpine Linux イメージをコピー
    system("cp -r ./image/alpine/* ./pivot_root_fs/");

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

    // ルートファイルシステムを変更するためのディレクトリを作成
    mkdir(NEW_ROOT "/old_root_fs", 0755);

    // pivot_root システムコールで root を変更
    if (syscall(SYS_pivot_root, NEW_ROOT, NEW_ROOT "/old_root_fs") == -1) {
        perror("pivot_root");
        return 1;
    }

    // カレントディレクトリを変更
    if (chdir("/") == -1) {
        perror("chdir");
        return 1;
    }

    // 古いルートファイルシステムをアンマウント
    if (umount2("/old_root_fs", MNT_DETACH) == -1) {
        perror("umount");
        return 1;
    }

    // ディレクトリを削除
    rmdir("/old_root_fs");

    // シェルを起動
    execlp("/bin/sh", "sh", NULL);

    return 0;
}
