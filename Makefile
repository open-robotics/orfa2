# -*- Makefile -*-
#
#

TARGET_CONFIG	?= OR_STM32F1_DS
#TARGET_CONFIG	?= simulator

ORFA	?= .

CSRC = $(LIBSRC) $(APPSRC) main.c

CPPSRC =

INCDIR = $(ORFA)/configs/$(TARGET_CONFIG) $(LIBINC) $(APPINC)

include $(ORFA)/configs/$(TARGET_CONFIG)/Makefile.inc
