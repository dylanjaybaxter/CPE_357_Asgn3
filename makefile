CC = gcc
CFLAGS = -Wall -g -ansi
LD = gcc
LDFLAGS = -g

all: hencode hdecode hencode.o hdecode.o huffman.o

hdecode: huffman.o hdecode.o
	$(LD) $(LDFLAGS) -o hdecode hdecode.o huffman.o
hencode: huffman.o hencode.o
	$(LD) $(LDFLAGS) -o hencode hencode.o huffman.o

hencode.o: hencode.c
	$(CC) $(CFLAGS) -c -o hencode.o hencode.c
hdecode.o: hdecode.c
	$(CC) $(CFLAGS) -c -o hdecode.o hdecode.c
huffman.o : huffman.c huffman.h
	$(CC) $(CFLAGS) -c -o huffman.o huffman.c

clean: hencode hdecode
	rm hencode.o hdecode.o huffman.o
debug: hencode hdecode
	./hencode st.txt out.out
	gdb hdecode
run: hencode hdecode
	./hencode st.txt out.out
	./hdecode out.out stNew.txt
valgrind: hencode hdecode
	valgrind -s --leak-check=full --show-leak-kinds=all ./hencode st.txt ste
	valgrind -s --leak-check=full --show-leak-kinds=all ./hdecode ste stOut

bintest: hencode hdecode
	./hencode foo out
	./hdecode out foo2
	od -tx1 foo > a
	od -tx2 foo2 > b
	diff -s a b
	rm foo
	rm foo2
