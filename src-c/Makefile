###############################################################
# Makefile for entire Edison C Library including Sample apps
# Copyright (c) 2014, Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU Lesser General Public License,
# version 2.1, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
# more details.
#
###############################################################

SUBDIRS = edison-lib/plugins/libmqttpubsub \
            edison-lib/plugins/libzmqpubsub \
            edison-lib/plugins/libzmqreqrep \
            sample-apps/mqtt-apps \
            sample-apps/zmq-apps

all: setup

clean:     
	@for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir; \
	done

setup:     
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

