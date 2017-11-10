# Copyright 2017 NXP

include config.mk

TOP_DIR=`pwd`
INSTALL_DIR ?= $(DESTDIR)
FIRMWARE_DIR="tools/firmware_generation"

ifndef KERNELDIR
$(error KERNELDIR is not set)
endif

ifneq ($(and $(EVB),$(BBMINI)),)
$(error EVB and BBMINI both defined)
endif

ifeq ($(or $(EVB),$(BBMINI)),)
$(error Neither EVB nor BBMINI are defined)
endif

ifdef EVB
BOARD="evb"
endif

ifdef BBMINI
BOARD="bbmini"
endif

all: modules

install: modules_install tools_install

clean: modules_clean tools_clean hal_clean install_clean

modules:
	$(MAKE) -C drivers/modules $(BOARD)

modules_install:
	$(MAKE) -C drivers/modules install_$(BOARD)

modules_clean:
	$(MAKE) -C drivers/modules clean


tools_install:
	install -d $(INSTALL_DIR)/lib/firmware
	cp -rf $(FIRMWARE_DIR)/*.bin $(INSTALL_DIR)/lib/firmware

tools_clean:
	find $(FIRMWARE_DIR) -name *.pyc | xargs rm -f
	find $(FIRMWARE_DIR) -name *.hex | xargs rm -f
	find $(FIRMWARE_DIR) -name *.bin | xargs rm -f

hal_clean:
	find hal/sja1105p/src/ -name *.o | xargs rm -f
	find hal/sja1105p/src/ -name *.cmd | xargs rm -f
	
install_clean:
	rm -rf $(INSTALL_TOP_DIR)

