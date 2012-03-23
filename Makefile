# -*- Makefile -*-
#
#

TARGET_CONFIG	?= OR_STM32F1_DS
#TARGET_CONFIG	?= simulator

ORFA	?= .

CSRC = $(COMSRC) $(SHSRC) main.c

CPPSRC =

INCDIR = $(ORFA)/configs/$(TARGET_CONFIG) $(COMINC) $(SHINC)

include $(ORFA)/configs/$(TARGET_CONFIG)/Makefile.inc
