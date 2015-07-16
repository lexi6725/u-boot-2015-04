#
# (C) Copyright 2000, 2001, 2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y	:= smdk6410.o
obj-$(CONFIG_SAMSUNG_ONENAND)	+= onenand.o
obj-y	+= lowlevel_init.o

ifdef CONFIG_SPL_BUILD
obj- := __dumy__.0
hostprogs-y	:= tools/mksmdk6410spl
always := $(HOSTPROGS-y)

$(obj)/tools/mksmdk6410spl: HOSTFLAGS:=$(filter-out -O2, $(HOSTCFLAGS))
endif
