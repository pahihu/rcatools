CFLAGS = -g #-DDIAGNOSTICS=1
OBJS = a18.o a18eval.o a18util.o

a18: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

test: a18
	a18 testa18.asm -L test18.prn -o test18.hex -b test18.bin

clean:
	$(RM) $(OBJS)
	$(RM) a18
	$(RM) test18.prn test18.hex test18.bin
