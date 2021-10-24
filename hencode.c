/*
main.c
Project: CPE 357 Lab03
Author: Dylan dybaxter
Description: This file contains the main functionality of a program that
implements huffman encoding
*/
#include<stdio.h>
#include<unistd.h>
#include<ctype.h>
#include<math.h>
#include<getopt.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include"huffman.h"
#define PERMS 0666

int main(int argc, char* const argv[]){
    /*If options included, exit*/
    int opt;
    while((opt = getopt(argc, argv,":n:")) != -1){
        perror("usage: htable filename");
        exit(EXIT_FAILURE);
    }
    /* For file*/
    int* freqs;
    int fdin = open(argv[1], O_RDONLY);
    if(fdin == -1){
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    /*Generate Character Histogram*/
    freqs = countFreq(fdin);

    /*Build Huffman Tree*/
    huff_Node* tree = createHTree(freqs);

    /*Generate Codes*/
    codeIndex* codeInd = genCodes(tree);

    /*Print Codes*/
    printCodes(codeInd);

    /*Open Write File*/
    int fdout = open(argv[2], O_WRONLY|O_CREAT, PERMS);
    if(fdout == -1){
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }

    /*Write Header*/
    writeHeader(fdout,freqs);

    /*Write Body*/
    writeBody(fdin, fdout, codeInd);

    /*close write file*/
    fdin = close(fdin);
    if(fdin == -1){
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }
    fdout = close(fdout);
    if(fdout == -1){
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    /*Free Memory*/
    free(freqs);
    freeTree(tree);
    freeIndex(codeInd);
    return 0;
}
