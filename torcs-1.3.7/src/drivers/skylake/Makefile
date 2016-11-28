##############################################################################
#
#    file                 : Makefile
#    created              : Mon 28 Nov 14:37:43 WET 2016
#    copyright            : (C) 2002 Diogo Silva
#
##############################################################################
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
##############################################################################

ROBOT       = skylake
MODULE      = ${ROBOT}.so
MODULEDIR   = drivers/${ROBOT}
SOURCES     = ${ROBOT}.cpp

SHIPDIR     = drivers/${ROBOT}
SHIP        = ${ROBOT}.xml car1-stock1.rgb logo.rgb
SHIPSUBDIRS = 

PKGSUBDIRS  = ${SHIPSUBDIRS}
src-robots-skylake_PKGFILES = $(shell find * -maxdepth 0 -type f -print)
src-robots-skylake_PKGDIR   = ${PACKAGE}-${VERSION}/$(subst ${TORCS_BASE},,$(shell pwd))

include ${MAKE_DEFAULT}
