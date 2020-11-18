CC = cc
CFLAGS = -O2
SRCS = 1802.c
OBJS = $(SRCS:.c=.o)

all: sim18

sim18: $(OBJS)
	$(CC) -o $@ $(OBJS)
	ls -l $@

rom.xxd: rom.txt
	awk -f cvt.awk rom.txt > rom.xxd

rom.bin: rom.xxd
	xxd -r rom.xxd rom.bin

rom.dis: rom.bin
	d18 -b0000 -e0200 -rrom.bin -2 > rom.dis

nucleus.bin: nucleus.asm
	a18 nucleus.asm -L nucleus.lst -b $@
	ls -l $@

clean:
	$(RM) sim18 $(OBJS)
	$(RM) rom.xxd rom.bin rom.dis
	$(RM) nucleus.lst nucleus.bin
