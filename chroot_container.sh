#!/bin/bash

cd $(dirname $0)

cp -r ./image/alpine ./chroot_fs
sudo chroot ./chroot_fs /bin/sh
