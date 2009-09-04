PROC=arm
TYPE=none-eabi
LDSCRIPT=ldscript

LIBPATH=/usr/local/$(PROC)-$(TYPE)/lib
INCPATH=/usr/local/$(PROC)-$(TYPE)/include
CC=$(PROC)-$(TYPE)-gcc
AS=$(PROC)-$(TYPE)-as
AR=$(PROC)-$(TYPE)-ar
LD=$(PROC)-$(TYPE)-ld
NM=$(PROC)-$(TYPE)-nm
OBJDUMP=$(PROC)-$(TYPE)-objdump
OBJCOPY=$(PROC)-$(TYPE)-objcopy
CPPFLAGS=
CFLAGS=-g -Os -c -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard

LEDblink: LEDblink.o Makefile $(LDSCRIPT) crt0.o
	$(LD) -g -v -T $(LDSCRIPT)  -Map $@.map -o $@ crt0.o $@.o
	$(NM) $@
	$(OBJCOPY) -O binary $@ $@.bin 
	$(OBJDUMP) -S -l $@ >$@.src

crt0.o:  Makefile $(LDSCRIPT) crt0.S
	$(CC) -g -c -o crt0.o crt0.S

clean:
	rm *.o
	rm LEDblink
