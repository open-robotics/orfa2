# -*- Makefile -*-
#
#

TARGET_CONFIG	?= OR_STM32F1_DS
#TARGET_CONFIG	?= simulator

ORFA	?= .

include ${ORFA}/ext/ros_lib/ros.mk

CSRC = $(LIBSRC) $(APPSRC) main.c

CPPSRC = $(ROSCPPSRC)

INCDIR = $(ORFA)/configs/$(TARGET_CONFIG) $(LIBINC) $(APPINC) $(ROSINC)

include $(ORFA)/configs/$(TARGET_CONFIG)/Makefile.inc
