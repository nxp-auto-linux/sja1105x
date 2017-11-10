# Copyright 2017 NXP

# Your local_config.mk should look like the follwing:
#export KERNELDIR=/home/user/linux_avb
#export STAGING_DIR=${SDKTARGETSYSROOT}
#export THIS_MOD_ROOTDIR=/home/user/nxp_switch

export CROSS_COMPILE?=aarch64-linux-gnu-
export KERNELDIR?=$(KERNEL_SRC)
export THIS_MOD_ROOTDIR?=$(shell pwd)
export INSTALL_TOP_DIR?=$(shell pwd)/build
export INSTALL_DIR?=$(INSTALL_TOP_DIR)/target
