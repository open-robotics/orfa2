# -*- Makefile -*-

CHIBIOS_REV= 	4052
CHIBIOS_SVNURL=	https://chibios.svn.sourceforge.net/svnroot/chibios/trunk


all: chibios


chibios:
	if [ ! -d "$@" ]; then \
		svn co -r $(CHIBIOS_REV) $(CHIBIOS_SVNURL) $@ ;\
	else \
		cd $@ && svn update -r $(CHIBIOS_REV) ;\
	fi

