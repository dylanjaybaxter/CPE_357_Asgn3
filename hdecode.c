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
    /*Open Read File, default to stdin if arg is -*/
    int* freqs;
    int fdin, fdout;
    if(*argv[1] != '-'){
        fdin = open(argv[1], O_RDONLY);
        if(fdin == -1){
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    else{
        fdin = 0;
    }

    /*Open Write File, default to stdout if no arg*/
    if(argv[2]){
        fdout = open(argv[2], O_WRONLY|O_CREAT, PERMS);
        if(fdout == -1){
            perror(argv[2]);
            exit(EXIT_FAILURE);
        }
    }
    else{
        fdout = 1;
    }

    /*Parse header of file*/
    freqs = parseHeader(fdin);

    /*Create HTree*/
    huff_Node* tree = createHTree(freqs);

    /*Create Codes*/
    codeIndex* codeInd = genCodes(tree);

    /*Write decoded test to file*/
    int sum = sumFreqs(freqs);
    int unique = isOneChar(freqs);
    if(unique == -1){
        decodeBody(fdin, fdout, codeInd, sum);
    }
    else{
        writeSingleChar(fdout, unique, sum);
    }
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
    free(freqs);
    freeTree(tree);
    freeIndex(codeInd);

    return 0;
}
