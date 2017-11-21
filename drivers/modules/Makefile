# Copyright 2017 NXP

ARCH=arm64

CFG_GEN_PATH = "../../tools/firmware_generation"
CFG_GEN_TOOL = SJA1105Q_firmware_sw
THIS_PATH = `pwd`

BOARDS := "bbmini evb"
SWITCHES_bbmini := 0 1 2
SWITCHES_evb := 0 1
TOTAL_SWITCHES_bbmini := 3
TOTAL_SWITCHES_evb := 2

# Arguments: 1: Switch number 2: Board
define build_firmware
	cd $(CFG_GEN_PATH); \
	python $(CFG_GEN_TOOL)$(1)_$(2).py; \
	cd $(THIS_PATH);
endef

# Arguments: 1: Total number of switches for this board
define install_driver
	$(MAKE) CROSS_COMPILE="$(CROSS_COMPILE)" ARCH=$(ARCH) -C $(KERNELDIR) M=`pwd` modules_install
	$(eval KERNEL_VERSION := $(shell cat $(KERNELDIR)/include/config/kernel.release))

	install -D sja1105.ko $(INSTALL_DIR)/lib/modules/$(KERNEL_VERSION)/kernel/drivers/spi/sja1105.ko
	$(foreach SWITCH,$(shell seq 1 $(1)),
		install -D $(CFG_GEN_PATH)/sja1105_cfg_mra2lt_$(SWITCH)-$(1).bin \
		           $(INSTALL_DIR)/lib/firmware/sja1105_cfg_mra2lt_$(SWITCH)-$(1).bin;)
	install -D sja1105.sh $(INSTALL_DIR)/home/root/sja1105.sh
endef

default:
	echo "Default target not supported!" && false

evb: modules firmware_evb

bbmini: modules firmware_bbmini

modules:
	$(MAKE) CROSS_COMPILE="$(CROSS_COMPILE)" V=1 ARCH=$(ARCH) -C $(KERNELDIR) M=`pwd`   $@

clean:
	$(RM) *.bin
	$(RM) *.hex
	$(MAKE) CROSS_COMPILE="$(CROSS_COMPILE)" V=1 ARCH=$(ARCH) -C $(KERNELDIR) M=`pwd`   $@

firmware_%:
	if ! echo $(BOARDS) | grep -q $* ; then echo Unknown board $*; false ; fi
	$(foreach SWITCH,$(SWITCHES_$*),$(call build_firmware,$(SWITCH),$*))

install_%:
	if ! echo $(BOARDS) | grep -q $* ; then echo Unknown board $*; false ; fi
	$(call install_driver,$(TOTAL_SWITCHES_$*))

obj-m +=  sja1105.o 

HAL_SRC_PATH =  ../../hal/sja1105p/src
HAL_INC_PATH  = $(THIS_MOD_ROOTDIR)/hal/sja1105p/inc
LLHAL_INC_PATH = $(THIS_MOD_ROOTDIR)/hal/sja1105p/inc/low_level_driver

EXTRA_CFLAGS+=  -I$(KERNELDIR)/include/linux -I$(HAL_INC_PATH)  -I$(LLHAL_INC_PATH)

ifneq ($(and $(EVB),$(BBMINI)),)
$(error EVB and BBMINI both defined)
endif

ifeq ($(or $(EVB),$(BBMINI)),)
$(error Neither EVB nor BBMINI are defined)
endif

ifdef EVB
EXTRA_CFLAGS+= -DEVB
endif

ifdef BBMINI
EXTRA_CFLAGS+= -DBBMINI
endif

sja1105-y := sja1105_spi.o sja1105_cfg_file.o hal_export.o
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_config.o
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_mgmtRoutes.o 
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_ptp.o 
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_vlan.o 
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_cbs.o
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_ethIf.o
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_portConfig.o
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_diagnostics.o
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_utils.o
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_addressResolutionTable.o
sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105P_configStream.o

sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105P_auxiliaryConfigurationUnit.o
sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105P_clockGenerationUnit.o
sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105P_resetGenerationUnit.o
sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105P_spi.o
sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105P_switchCore.o
sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105P_sgmii.o


#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_config.o
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_mgmtRoutes.o 
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_ptp.o 
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_vlan.o 
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_cbs.o
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_ethIf.o
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_portConfig.o
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_diagnostics.o
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_utils.o
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_addressResolutionTable.o
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_configStream.o
#sja1105-y += $(HAL_SRC_PATH)/NXP_SJA1105_crc.o

#sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105_auxiliaryConfigurationUnit.o
#sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105_clockGenerationUnit.o
#sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105_resetGenerationUnit.o
#sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105_spi.o
#sja1105-y += $(HAL_SRC_PATH)/low_level_driver/NXP_SJA1105_switchCore.o








