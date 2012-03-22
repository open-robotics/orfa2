# -*- Makefile -*-
#
#

TARGET_BOARD	?= OR_STM32F1_DS
#TARGET_BOARD	?= simulator

ORFA	?= .

CSRC = $(COMSRC) $(SHSRC) main.c

CPPSRC =

INCDIR = $(ORFA)/configs/$(TARGET_BOARD) $(COMINC) $(SHINC)

include $(ORFA)/configs/$(TARGET_BOARD)/Makefile.inc
