CONFIG_SYS_CONFIG_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))

ccflags-y += -I$(srctree)/$(src)/$(CONFIG_SYS_CONFIG_NAME)
ccflags-y += -I$(srctree)/$(src)

obj-y += $(CONFIG_SYS_CONFIG_NAME)/mhal_gpio.o
obj-y += drvGPIO.o
obj-y += mdrv_sw_iic.o

