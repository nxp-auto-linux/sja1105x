MYPLATFORM=bbmini
#NUMBER_SWITCHES

#Platform dependent SPI parameters
#SPI_FREQ
#SPI_SWAP
#SPI_BPW
#SPI_BPW_MSG
#NR_CFG_BLOCKS


ifeq ($(MYPLATFORM), imx)
	# i.MX
	NUMBER_SWITCHES ?= 1
	SPI_FREQ ?= 25000000
	SPI_SWAP ?= 0
	SPI_BPW ?= 32
	SPI_BPW_MSG ?= "(t.len << 3)"
	NR_CFG_BLOCKS ?= 6

	MYTOOLCHAIN=arm-linux-gnueabi-
	MYARCHITECTURE=arm
	MYCOMPILER=arm-linux-gnueabi-gcc-4.7
	KERNELDIR ?= /home/marco/Documents/linux_automotive/linux_avb
else ifeq ($(MYPLATFORM), evb)
	# S32V234-EVB
	NUMBER_SWITCHES ?= 2
	SPI_FREQ ?= 12000000
	SPI_SWAP ?= 1
	SPI_BPW ?= 16
	SPI_BPW_MSG ?= 16
	NR_CFG_BLOCKS ?= 1

	MYTOOLCHAIN=aarch64-linux-gnu-
	MYARCHITECTURE=arm64
	MYCOMPILER=aarch64-linux-gnu-gcc
	KERNELDIR ?= ~/work/linux
else ifeq ($(MYPLATFORM), bbmini)
	# S32V234 BlueBox Mini
	NUMBER_SWITCHES ?= 3
	SPI_FREQ ?= 12000000
	SPI_SWAP ?= 1
	SPI_BPW ?= 16
	SPI_BPW_MSG ?= 16
	NR_CFG_BLOCKS ?= 1

	MYTOOLCHAIN=aarch64-linux-gnu-
	MYARCHITECTURE=arm64
	MYCOMPILER=aarch64-linux-gnu-gcc
	KERNELDIR ?= /home/marco/Documents/yocto_auto_linux_bsp15.0/build_s32v234bbmini_release/tmp/work/s32v234bbmini-fsl-linux/linux-s32v2xx/4.1.26-r0/git
else ifeq ($(MYPLATFORM), gplat)
	# S32G-PROCEVB-S plus S32GRV-PLATEVB
	NUMBER_SWITCHES ?= 1
	SPI_FREQ ?= 12000000
	SPI_SWAP ?= 1
	SPI_BPW ?= 16
	SPI_BPW_MSG ?= 16
	NR_CFG_BLOCKS ?= 1

	MYTOOLCHAIN=aarch64-linux-gnu-
	MYARCHITECTURE=arm64
	MYCOMPILER=aarch64-linux-gnu-gcc

	DISABLE_SWITCHDEV = 1
	DISABLE_HOST_NETDEV = 1
else
        $(error Unsupported platform specified!)
endif

FIRMWARE_DIR="tools/firmware_generation"

.PHONY: all modules clean firmware firmware_clean

all: modules firmware

modules:
	$(MAKE) ARCH=$(MYARCHITECTURE) CROSS_COMPILE=$(MYTOOLCHAIN) CC=$(MYCOMPILER) -C $(KERNELDIR) M=`pwd` modules

clean: firmware_clean
	$(MAKE) ARCH=$(MYARCHITECTURE) CROSS_COMPILE=$(MYTOOLCHAIN) CC=$(MYCOMPILER) -C $(KERNELDIR) M=`pwd` $@

modules_install: firmware_install
	$(eval KERNEL_VERSION := $(shell cat $(KERNELDIR)/include/config/kernel.release))
	install -D sja1105pqrs.ko $(INSTALL_DIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/spi/sja1105pqrs.ko

firmware:
	$(MAKE) -C $(FIRMWARE_DIR) BOARD=$(MYPLATFORM) NR_SWITCHES=$(NUMBER_SWITCHES)

firmware_install:
	@mkdir -p $(INSTALL_DIR)/lib/firmware/
	@$(foreach SWITCH, $(shell seq 1 $(NUMBER_SWITCHES) ), \
		install -D $(FIRMWARE_DIR)/sja1105p_$(SWITCH)-$(NUMBER_SWITCHES)_cfg.bin \
			$(INSTALL_DIR)/lib/firmware/;)

firmware_clean:
	$(MAKE) -C $(FIRMWARE_DIR) clean


obj-m +=  sja1105pqrs.o

#include and src paths
APP_SRC_PATH   = app/src
APP_INC_PATH   = $(src)/app/inc
INT_SRC_PATH   = platform_integration/src
INT_INC_PATH   = $(src)/platform_integration/inc
INDEP_SRC_PATH = platform_independent/src
INDEP_INC_PATH = $(src)/platform_independent/inc
INDEP_LL_SRC_PATH = platform_independent/src/low_level_driver
INDEP_LL_INC_PATH = $(src)/platform_independent/inc/low_level_driver
SWDEV_SRC_PATH = switchdev/src
SWDEV_INC_PATH = $(src)/switchdev/inc

PLATFORM_DEPENDENT  = -D SJA1105P_N_SWITCHES=$(NUMBER_SWITCHES)
PLATFORM_DEPENDENT += -D SPI_FREQUENCY=$(SPI_FREQ) -D SPI_SWITCH_WORDS=$(SPI_SWAP) -D SPI_BITS_PER_WORD=$(SPI_BPW) -D SPI_BITS_PER_WORD_MSG=$(SPI_BPW_MSG) -D SPI_CFG_BLOCKS=$(NR_CFG_BLOCKS)
ifdef DISABLE_SWITCHDEV
	PLATFORM_DEPENDENT += -D DISABLE_SWITCHDEV
endif
ifdef DISABLE_HOST_NETDEV
	PLATFORM_DEPENDENT += -D DISABLE_HOST_NETDEV
endif

EXTRA_CFLAGS+=  -I$(KERNELDIR)/include/linux -I$(INDEP_INC_PATH) -I$(INDEP_LL_INC_PATH) -I$(INT_INC_PATH) -I$(APP_INC_PATH) -I$(SWDEV_INC_PATH) -I$(src) $(PLATFORM_DEPENDENT)

#Top level
sja1105pqrs-y := sja1105p_init.o

#platform integration
sja1105pqrs-y += $(INT_SRC_PATH)/sja1105p_spi_linux.o

#apps
sja1105pqrs-y += $(APP_SRC_PATH)/sja1105p_cfg_file.o
sja1105pqrs-y += $(APP_SRC_PATH)/sja1105p_debugfs.o
sja1105pqrs-y += $(APP_SRC_PATH)/sja1105p_general_status.o
sja1105pqrs-y += $(APP_SRC_PATH)/sja1105p_hal_export.o

#platform independent
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_config.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_cbs.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_mgmtRoutes.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_ptp.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_vlan.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_ethIf.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_portConfig.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_utils.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_addressResolutionTable.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_configStream.o
sja1105pqrs-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_diagnostics.o

#low level driver
sja1105pqrs-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_auxiliaryConfigurationUnit.o
sja1105pqrs-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_clockGenerationUnit.o
sja1105pqrs-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_resetGenerationUnit.o
sja1105pqrs-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_sgmii.o
sja1105pqrs-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_spi.o
sja1105pqrs-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_switchCore.o

#switchdev
ifndef DISABLE_SWITCHDEV
	sja1105pqrs-y += $(SWDEV_SRC_PATH)/sja1105p_switchdev.o
endif
