# -*- Makefile -*-

CHIBIOS_REV= 	4258
CHIBIOS_SVNURL=	https://chibios.svn.sourceforge.net/svnroot/chibios/trunk


all: chibios ros_lib


chibios:
	if [ ! -d "$@" ]; then \
		svn co -r $(CHIBIOS_REV) $(CHIBIOS_SVNURL) $@ ;\
	else \
		cd $@ && svn update -r $(CHIBIOS_REV) ;\
	fi


ros_lib:
	rosrun rosserial_client make_library.py ./ orfa2_msgs nav_msgs

