# $Id$

#############################################################################
#
# Copyright 2002 and 2005
# Nanotron Technologies
#
# Author: S. Rohdemann and B. Jozefini
#
# Description :
#    This file contains the rules for the nanoNET ATMega128 board portation
#
#############################################################################

#
# $Log$
#

#
# General Compiler/Linker options
#
CROSS_COMPILE = msp430-
AS			= $(CROSS_COMPILE)as
LD			= $(CROSS_COMPILE)ld
CC			= $(CROSS_COMPILE)gcc
AR			= $(CROSS_COMPILE)ar

MAINEXE = main

ELF := $(MAINEXE).elf
HEX := $(MAINEXE).hex
#EXTCOFF := $(MAINEXE).cof
#EEP := $(MAINEXE).eep
LSS := $(MAINEXE).lss
SYM := $(MAINEXE).sym
MAP := $(MAINEXE).map

#
# General Compiler/Linker options
#
INC	        := -I./h
#OPTFLAGS	:= -Os -fomit-frame-pointer
#DEBUGFLAGS	:= -gstabs
#FLAGS		:= -O2 -Wall -Wshadow -save-temps
FLAGS		:= -O2 -W -Wall -Wshadow \
#					 -Wno-trigraphs \
	         -fno-strict-aliasing \
					 -Wstrict-prototypes \
					 -fno-common
CFLAGS		:= $(FLAGS) $(DEBUGFLAGS) $(INC) -DTRACE_DEBUG
#AFLAGS		:= -D__ASSEMBLY__ $(CPPFLAGS)
#LFLAGS		:= -Wl,-Map=$(MAP),--cref
#LFLAGS		:= -Wl,-Map=$(MAP)
#LFLAGS		+= -Wl,-u,vfprintf 
#-lprintf_flt

ifeq ($(mode), anchor)
CFLAGS += -DRANGE_DEMO_ANCHOR
else
ifeq ($(mode), tag)
CFLAGS += -DRANGE_DEMO_TAG
endif
endif

CFLAGS += -DUSE_UCIRQ


CCVERSION	:= $(shell $(CC) --verbose 2>&1 | tail -1)

#
# Special Cross-Compile Options
#
CFLAGS		+= -mmcu=msp430x1611 \
#				-funsigned-char -funsigned-bitfields \
				-fpack-struct -fshort-enums
#EXTRA_LIBS	+= -lm

OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
NM := $(CROSS_COMPILE)nm
SIZE = $(CROSS_COMPILE)size

# Output format
FORMAT = ihex

# Convert ELF to COFF for use in debugging / simulating in AVR Studio.
#COFFCONVERT := $(OBJCOPY) --debugging \
#--change-section-address .data-0x800000 \
#--change-section-address .bss-0x800000 \
#--change-section-address .noinit-0x800000 \
#--change-section-address .eeprom-0x810000

OBJS =  main.o led.o dco.o delay.o util.o\
	   usart.o \
	   i2c.o \
	   bm.o \
	   ina219.o \
	   adc.o \
	   compass.o \
	   gps.o \
	   timer.o \
	   hwclock.o \
	   ntrxinit.o \
	   nnspi.o \
	   ntrxutil.o \
	   ntrxiqpar.o \
	   ntrxdil.o \
	   ntrxranging.o \
	   app.o \
	   hbm2x1m.o

		 
#all:	$(HEX) $(EXTCOFF) $(EEP) $(LSS) $(SYM) elfsize
all:	$(HEX) elfsize elfsize2 tags mkcsp

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS)  -o $@ $(OBJS) $(EXTRA_LIBS)

%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) $< $@

#%.cof: %.elf
#	$(COFFCONVERT) -O coff-ext-avr $< $@

#%.eep: %.elf
#	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
#	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

%.lss: %.elf
	$(OBJDUMP) -h -S $< > $@

%.sym: %.elf
	$(NM) -n $< > $@

%.o:	%.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $<

elfsize:
	@if [ -f $(ELF) ]; then $(SIZE) -A $(ELF); fi

elfsize2:
	@if [ -f $(ELF) ]; then $(OBJDUMP) -h $(ELF) | awk '$$2 ~ /.(text|data|bss)/ { a[$$2]=strtonum("0x"$$3)     }; END { printf "\tFlash:\t%8d bytes\n\tRAM:\t%8d bytes\n\n",a[".text"],a[".data"]+a[".bss"] };'; fi

clean:
	-rm -f $(OBJS) $(ELF) $(HEX) $(EXTCOFF) $(EEP) $(LSS) $(SYM) $(MAP) *.i *.s tags cscope.out cscope.files

burn:
	tos-bsl --telosb -c /dev/ttyUSB$(tty) -r -e -I -p $(HEX)

burn2:
	tos-bsl --telosb -c /dev/ttyUSB$(tty) -r -e -I -p $(hex)

tags:
	ctags -R
mkcsp:
	mkcscope.sh

.PHONY: all clean elfsize burn tags

ff:
	@ff $(find)

#mkcscope.sh
