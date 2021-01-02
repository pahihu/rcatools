CC = cc -Wall #-g
CFLAGS = -O2
SRCS = 1802.c fds.c curterm.c
OBJS = $(SRCS:.c=.o)

all: sim18 ut20.rom fforth.idi

sim18: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	ls -l $@

#
# GLL-MAG ROM
#
rom.xxd: rom.txt
	awk -f cvt.awk rom.txt > rom.xxd

rom.bin: rom.xxd
	xxd -r rom.xxd rom.bin

rom.dis: rom.bin
	d18 -b0000 -e0200 -rrom.bin -2 > rom.dis

#
# microFORTH nucleus
#
nucleus.bin: nucleus.asm
	a18 nucleus.asm -L nucleus.lst -b $@
	ls -l $@

#
# fig-FORTH
#
fforth.idi: fforth.sr
	a18 fforth.sr -r -m $@

#
# UT20 ROM
#
ut20p_1.bin: ut20p_1.sr
	a18 ut20p_1.sr -r -f 0C4H -L ut20p_1.lst -b ut20p_1.bin
	a18 ut20p_1.sr -r -f 0C4H -L ut20p_1.lst -o ut20p_1.hex

ut20p_2.bin: ut20p_2.sr
	a18 ut20p_2.sr -r -f 0C4H -L ut20p_2.lst -b ut20p_2.bin
	a18 ut20p_2.sr -r -f 0C4H -L ut20p_2.lst -o ut20p_2.hex

ut20.rom: ut20p_1.bin ut20p_2.bin
	cat ut20p_1.bin ut20p_2.bin >ut20.rom

clean:
	$(RM) sim18 $(OBJS)
	$(RM) rom.xxd rom.bin rom.dis
	$(RM) nucleus.lst nucleus.bin
	$(RM) fforth.idi
	$(RM) ut20.rom
	$(RM) ut20p_1.lst ut20p_1.bin ut20p_1.hex
	$(RM) ut20p_2.lst ut20p_2.bin ut20p_2.hex
