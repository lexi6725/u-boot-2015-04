#
# Copyright (C) 2009 Samsung Electronics
# Minkyu Kang <mk7.kang@samsung.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y		+= cpu_info.o
obj-y	+= cache.o
obj-y	+= reset.o
obj-y	+= clock.o
ifndef CONFIG_SPL_BUILD
obj-y		+= timer.o
else
obj-y	+= generic.o
endif
