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
#include<sys/types.h>
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
    /*Open Read File*/
    int* freqs;
    int fdin = open(argv[1], O_RDONLY);
    if(fdin == -1){
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    /*Open Write File*/
    int fdout = open(argv[2], O_WRONLY|O_CREAT, PERMS);
    if(fdout == -1){
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }

    /*Parse header of file*/
    freqs = parseHeader(fdin);

    /*Create HTree*/
    huff_Node* tree = createHTree(freqs);

    /*Create Codes*/
    codeIndex* codeInd = genCodes(tree);

    /*Write decoded test to file*/
    int sum = sumFreqs(freqs);
    decodeBody(fdin, fdout, codeInd, sum);

    /*Close Read file*/
    opt = 0;
    opt = close(fdin);
    if(opt == -1){
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }
    /*Close Write file*/
    opt = close(fdout);
    if(opt == -1){
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    /*Free Memory*/
    return 0;
}
