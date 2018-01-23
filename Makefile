MYPLATFORM=bbmini
#NUMBER_SWITCHES

#Platform dependent SPI parameters
#SPI_FREQ
#SPI_SWAP
#SPI_BPW
#SPI_BPW_MSG
#NR_CFG_BLOCKS


ifeq ($(MYPLATFORM), imx)
	#i.MX
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
else ifeq ($(MYPLATFORM), bbmini)
	#bbmini
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
else
        $(error Unsupported platform specified!)
endif

all:
	$(MAKE) ARCH=$(MYARCHITECTURE) CROSS_COMPILE=$(MYTOOLCHAIN) CC=$(MYCOMPILER) -C $(KERNELDIR) M=`pwd` modules

clean:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- CC=arm-linux-gnueabi-gcc-4.7 -C $(KERNELDIR) M=`pwd` $@


obj-m +=  sja1105.o

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

EXTRA_CFLAGS+=  -I$(KERNELDIR)/include/linux -I$(INDEP_INC_PATH) -I$(INDEP_LL_INC_PATH) -I$(INT_INC_PATH) -I$(APP_INC_PATH) -I$(SWDEV_INC_PATH) -I$(src) $(PLATFORM_DEPENDENT)

#Top level
sja1105-y := sja1105_init.o

#platform integration
sja1105-y += $(INT_SRC_PATH)/sja1105_spi_linux.o

#apps
sja1105-y += $(APP_SRC_PATH)/sja1105_cfg_file.o
sja1105-y += $(APP_SRC_PATH)/sja1105_debugfs.o
sja1105-y += $(APP_SRC_PATH)/sja1105_general_status.o
sja1105-y += $(APP_SRC_PATH)/hal_export.o

#platform independent
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_config.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_cbs.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_mgmtRoutes.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_ptp.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_vlan.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_ethIf.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_portConfig.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_utils.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_addressResolutionTable.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_configStream.o
sja1105-y += $(INDEP_SRC_PATH)/NXP_SJA1105P_diagnostics.o

#low level driver
sja1105-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_auxiliaryConfigurationUnit.o
sja1105-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_clockGenerationUnit.o
sja1105-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_resetGenerationUnit.o
sja1105-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_sgmii.o
sja1105-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_spi.o
sja1105-y += $(INDEP_LL_SRC_PATH)/NXP_SJA1105P_switchCore.o

#switchdev
sja1105-y += $(SWDEV_SRC_PATH)/sja1105p_switchdev.o
