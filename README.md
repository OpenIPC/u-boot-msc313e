# u-boot-msc313e
U-Boot for msc313e SoC

## Building
```export ARCH=arm```

```export CROSS_COMPILE=/opt/arm-linux-gnueabihf-4.8.3-201404/bin/arm-linux-gnueabihf- ```

```make infinityb3_defconfig```

```make -j5```

```./make_boot.sh (optionally)```
