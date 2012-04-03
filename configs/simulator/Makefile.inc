# -*- Makefile -*-
##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -std=gnu99 -O0 -ggdb -fomit-frame-pointer
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = 
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT =
endif

# Enable this if you want the linker to remove unused code and data
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

#
# Build global options
##############################################################################

##############################################################################
# Project, sources and paths
#

# Define project name here
PROJECT = orfa2

# Imported source files and paths
ORFA    ?= .
CHIBIOS ?= $(ORFA)/ext/chibios
include $(ORFA)/boards/simulator/board.mk
include ${CHIBIOS}/os/hal/platforms/Posix/platform.mk
include ${CHIBIOS}/os/hal/hal.mk
include ${CHIBIOS}/os/ports/GCC/SIMIA32/port.mk
include ${CHIBIOS}/os/kernel/kernel.mk
include $(ORFA)/hal/platforms/Posix/platform.mk
include $(ORFA)/hal/hal.mk
include $(ORFA)/lib/lib.mk
include $(ORFA)/app/apps.mk

# Define linker script file here
LDSCRIPT=

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC += $(PORTSRC) \
       $(KERNSRC) \
       $(TESTSRC) \
       $(HALSRC) \
       $(PLATFORMSRC) \
       $(BOARDSRC) \
       $(CHIBIOS)/os/various/evtimer.c \
       $(CHIBIOS)/os/various/chprintf.c

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC +=

INCDIR += $(PORTINC) $(KERNINC) \
         $(HALINC) $(PLATFORMINC) $(BOARDINC) \
         $(CHIBIOS)/os/various

#
# Project, sources and paths
##############################################################################

##############################################################################
# Compiler settings
#

TRGT = 
CC   = $(TRGT)gcc
CPPC = $(TRGT)g++
LD   = $(TRGT)gcc
#LD   = $(TRGT)g++
AS   = $(TRGT)gcc -x assembler-with-cpp

# Define C warning options here
CWARN = -Wall -Wextra -Wstrict-prototypes

# Define C++ warning options here
CPPWARN = -Wall -Wextra

#
# Compiler settings
##############################################################################

##############################################################################
# Start of default section
#

# List all default C defines here, like -D_DEBUG=1
DDEFS = -DSIMULATOR -DSHELL_USE_IPRINTF=FALSE

# List all default ASM defines here, like -D_DEBUG=1
DADEFS =

# List all default directories to look for include files here
DINCDIR =

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS =

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS =

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

#
# End of user defines
##############################################################################################

include $(ORFA)/configs/simulator/rules.mk
