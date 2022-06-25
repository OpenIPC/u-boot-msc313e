#!/bin/sh

dd if=/dev/zero of=BOOT.bin bs=1024  count=16 seek=0
dd if=MXP_SF.bin of=BOOT.bin bs=1024  count=4 seek=12
dd if=IPL.bin of=BOOT.bin bs=1024  count=48 seek=16
dd if=IPL_CUST.bin of=BOOT.bin bs=1024  count=60 seek=64
dd if=/dev/zero of=BOOT.bin bs=1024  count=4 seek=124
dd if=/dev/zero of=BOOT.bin bs=1024  count=60 seek=132
dd if=u-boot.xz.img.bin of=BOOT.bin bs=1024  count=128 seek=128
