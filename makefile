CC = gcc
CFLAGS = -Wall -g -pg -ansi
LD = gcc
LDFLAGS = -g -pg

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

test: htable
	./htable st.txt > stOut.txt
	~pn-cs357/demos/htable st.txt > stSol.txt
	diff -s stOut.txt stSol.txt
testman: htable
	./htable /usr/share/man/*/* > outme.txt
	~pn-cs357/demos/htable /usr/share/man/*/* > outsol.txt
	diff -s outme.txt outsol.txt
testwap: htable
	./htable WAP.txt > WAPO.txt
	~pn-cs357/demos/htable WAP.txt > WAPI.txt
	diff -s WAPO.txt WAPI.txt
testone: htable
	./htable oc > ocOut.txt
	~pn-cs357/demos/htable oc > ocSol.txt
	diff -s ocOut.txt ocSol.txt
