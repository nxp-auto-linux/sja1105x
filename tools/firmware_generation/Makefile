# Copyright NXP 2018

BOARD ?= evb
NR_SWITCHES ?= 2

PYTHON=python
CFG_GEN_TOOL = SJA1105Q_firmware_sw

.PHONY: all firmware clean

all: firmware

firmware:
	@$(foreach SWITCH, $(shell seq 0 $$(( $(NR_SWITCHES) - 1 )) ), \
		$(PYTHON) $(CFG_GEN_TOOL)$(SWITCH)_$(BOARD).py;)

clean:
	@$(RM) *.pyc
	@$(RM) *.bin
	@$(RM) *.hex
