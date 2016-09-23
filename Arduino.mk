# -*- GNUMakefile -*-
ROOT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
export ARDMK_DIR := $(ROOT_DIR)Arduino-Makefile

include $(ARDMK_DIR)/Arduino.mk
