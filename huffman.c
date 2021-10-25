/*
huffman.c
Project: CPE 357 Assignment3
Author: Dylan (dybaxter)
Description: This file contains functions and structures necessary for
huffman encoding and decoding
*/
#include<unistd.h>
#include<stdio.h>
#include<stdint.h>
#include<arpa/inet.h>
#include"huffman.h"

/*------------------------- Constructors---------------------------*/
Node_List* init_Node_List(){
    Node_List* list = (Node_List*)malloc(sizeof(Node_List));
    list->items = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

huff_Node* init_Huff_Node(int freq, int c){
    huff_Node* newNode = (huff_Node*)malloc(sizeof(huff_Node));
    newNode->freq = freq;
    newNode->c = c;
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->right = NULL;
    newNode->left = NULL;
    return newNode;
}

codeIndex* init_CodeIndex(){
    int* codes = (int*)malloc(256*sizeof(int));
    int* lens = (int*)malloc(256*sizeof(int));
    int i;
    for(i = 0; i<256;i++){
        codes[i] = 0;
        lens[i] = 0;
    }
    codeIndex* index = (codeIndex*)malloc(sizeof(codeIndex));
    index->codes = codes;
    index->lens = lens;
    return index;
}

/*------------------------- Deconstructors---------------------------*/
void freeIndex(codeIndex* index){
    free(index->codes);
    free(index->lens);
    free(index);
}

void freeList(Node_List* list){
    /*if((list->head != NULL) && (list->tail != NULL)){
        while(list->head != list->tail){
            free(list->tail);
            list->tail = (huff_Node*)list->tail->next;
        }
        free(list->tail);
    }*/
    free(list);
}

void freeTree(huff_Node* tree){
    /*If tree exisists*/
    if(tree != NULL){
        /*Free children recursively*/
        if((tree->right != NULL) && (tree->left !=NULL)){
            huff_Node* right = tree->right;
            huff_Node* left = tree->left;
            freeTree(left);
            freeTree(right);
        }
        /*Free Self*/
        free(tree);
    }
}

/*------------------------- List Functions---------------------------*/
Node_List* createNodeList(int* freqs){
    /*Allocate Struct Array and List*/
    Node_List* list = init_Node_List();
    /*Copy over node information*/
    int i = 0;
    for(;i<256;i++){
        /*Insert non zero characters*/
        if(freqs[i] != 0){
            huff_Node* newNode = init_Huff_Node(freqs[i], i);
            insertNode(list, newNode);
        }
    }
    /* Return List */
    return list;
}

int compareNodes(huff_Node* node1, huff_Node* node2){
    /* Compare Frequencies*/
    if(node1->freq > node2->freq){
        return 1;
    }
    else if(node1->freq < node2->freq){
        return -1;
    }
    else{
        /* Compare Characters*/
        if(node1->c > node2->c){
            return 1;
        }
        else if(node1->c < node2->c){
            return -1;
        }
        else{
            return 0;
        }
    }
}

void insertNode(Node_List* list, huff_Node* node){
    /* If list is empty*/
    if(list->head==NULL && list->tail==NULL){
        list->head = node;
        list->tail = node;
        list->items = 1;
    }
    else{
        /*If list is not empty*/
        huff_Node* currNode = list->tail;
        huff_Node* prevNode = NULL;
        while(1){
            /*If current node is greater than node
            Insert at the back of current node*/
            if(compareNodes(currNode, node) >= 0){
                /*If not 1st item*/
                if(currNode->prev != NULL){
                    /*link previous node to inserted*/
                    prevNode = (huff_Node*)currNode->prev;
                    prevNode->next = node;
                    node->prev = prevNode;
                }
                else{
                    /*inserted must be new tail*/
                    list->tail = node;
                }
                /*link inserted behind current node*/
                currNode->prev = node;
                node->next = currNode;

                /*If end of list*/
                list->items = list->items + 1;
                break;
            }
            else if(compareNodes(currNode, node) < 0){
                /*If node is greater than current, move to next*/
                if(currNode->next == NULL){
                    currNode->next = node;
                    node->prev = currNode;
                    list->head = node;
                    list->items = list->items + 1;
                    break;
                }
                currNode = (huff_Node*)currNode->next;
            }
            else{
                /*if equal*/
                /*link inserted behind current node*/


                break;
            }
        }
    }
}
/*Pops the minimum value from the stack as node*/
huff_Node* popMin(Node_List* list){
    /*If list is not empty*/
    if(list->tail != NULL){
        /*Save last*/
        huff_Node* minim = list->tail;
        /*If not the only node, remove reference from next node and set tail*/
        if(minim->next != NULL){
            minim->next->prev = NULL;
            list->tail = minim->next;
        }
        else{
            /*Empty List*/
            list->head = NULL;
            list->tail = NULL;
        }
        /*Decrement Items*/
        list->items = list->items -1;
        return minim;
    }
    else{
        return NULL;
    }
}


/*------------------------- Huffman Functions---------------------------*/
/*Counts Frequency of characters from 0 to 255 and outputs an ordered list*/
int* countFreq(int fdin){
    /* Initialize char array */
    int* freqs = (int*)malloc(256*sizeof(int));
    uint8_t i;
    for(i = 0; i<256;i++){
        freqs[i] = 0;
    }

    /* Iterate until EOF, incrmenting chararters as found*/
    int c = 0;
    int numRead;
    unsigned char buff[BUFF_SIZE];
    while((numRead = read(fdin, buff, BUFF_SIZE)) > 0){
        for(i=0;i<numRead;i++){
            c = buff[i];
            freqs[c] = freqs[c] + 1;
        }
    }
    if(numRead == -1){
        perror("Freq Read");
        exit(EXIT_FAILURE);
    }

    return freqs;
}

/*Creates a huffman tree frm list of freqs*/
huff_Node* createHTree(int* freqs){
    /*Initialize node list(add non-zero characters)*/
    Node_List* list = createNodeList(freqs);
    huff_Node* node1 = NULL;
    huff_Node* node2 = NULL;
    huff_Node* sumNode = NULL;
    int sum = 0;

    /*Until 1 node remains,*/
    while(list->head != list->tail){
        /*Pop 2 nodes*/
        node1 = popMin(list);
        node2 = popMin(list);

        /*Attach both to sum node */
        sum = (node1->freq)+(node2->freq);
        sumNode = init_Huff_Node(sum, 0);
        sumNode->left = node1;
        sumNode->right = node2;

        /*Reinsert node into list*/
        insertNode(list, sumNode);
    }
    /*Return Remaining Node*/
    huff_Node* tree = popMin(list);
    freeList(list);
    return tree;
}

void genCodesRecur(huff_Node* tree, codeIndex* index, int code, int len){
    /*If leaf*/
    if((tree->left == NULL)&&(tree->right == NULL)){
        if(len == 0){
            len = 1;
            code = -1;
        }
        index->codes[tree->c] = code;
        index->lens[tree->c] = len;
    }
    else{
        /*Traverse Right*/
        genCodesRecur(tree->right, index, (code*2)+1, len+1);
        /*Traverse Left*/
        genCodesRecur(tree->left, index, code*2, len+1);
    }
}

/*Creates huffman codes from tree*/
codeIndex* genCodes(huff_Node* tree){
    /*Initialize code  and bit count*/
    codeIndex* index = init_CodeIndex();
    if(tree != NULL){
        /*Recursively Transverse Tree*/
        genCodesRecur(tree, index, 0, 0);
    }
    return index;
}

void printCodes(codeIndex* codeInd){
    unsigned int i;
    int j;
    unsigned int comp = 0;
    for(i=0;i<256;i++){
        if(codeInd->lens[i] != 0 ){
            /*Print bits*/
            printf("0x%02x: ", i);

            /*Create max for max bit*/
            comp = 1;
            for(j=0;j<codeInd->lens[i]-1;j++){
                comp = comp*2;
            }

            /*If code is not nothing*/
            if(codeInd->codes[i]!=-1){
                /* Print bits while shift mask right*/
                for(j=0;j<codeInd->lens[i];j++){
                    if(comp&codeInd->codes[i]){
                        printf("1");
                    }
                    else{
                        printf("0");
                    }
                    comp = comp>>1;
                }
            }
            printf("\n");
        }
    }
}

/*Write header to file*/
void writeHeader(int fdout, int* hist){
    /*Count number of unique chars*/
    int i;
    uint8_t i8;
    uint32_t ordered = 0;
    uint8_t count = 0;
    for(i=0;i<256;i++){
        if(hist[i] != 0){
            count++;
        }
    }
    count--;
    /*Write number chars to file */
    if(write(fdout,&count,1)== -1){
        perror("write count total");
        exit(EXIT_FAILURE);
    }
    /*For each unique character in the file */
    for(i=0;i<256;i++){
        if(hist[i] != 0){
            /*write character byte*/
            i8 = (uint8_t)i;
            if(write(fdout,&i8,1)== -1){
                perror("write char");
                exit(EXIT_FAILURE);
            }
            /*Write 256 bit code*/
            ordered = htonl(hist[i]);
            if(write(fdout,&ordered,4)== -1){
                perror("write count");
                exit(EXIT_FAILURE);
            }
        }
    }
}

/*Write Body to File*/
void writeBody(int fdin, int fdout, codeIndex* codeInd){
    /*Reset read pointer to beginning of file*/
    lseek(fdin,0,SEEK_SET);
    /*For each byte*/
    int numRead; /*for number of bytes read*/
    int code; /* current code*/
    int len;/*length of code*/
    unsigned int comp = 0; /*Mask for extracting code*/
    uint8_t bitBuff = 0; /*Buffer for writing bits*/
    int buffCount = 0; /*Number of bits in buffer*/
    char buff[BUFF_SIZE]; /*buffer for reading chars*/
    int i; /*For loop 1*/
    int j; /*For loop 1*/
    while((numRead = read(fdin, buff, BUFF_SIZE)) > 0){
        for(i=0;i<numRead; i++){
            /*Find code for char*/
            code = codeInd->codes[(int)buff[i]];
            len = codeInd->lens[(int)buff[i]];
            /*Write code for char*/
            /*Initialize Comparator*/
            comp = 1;
            for(j=0;j<len-1;j++){
                comp = comp*2;
            }
            if(code!=-1){
                /* Fill Buffer with bits in correct order*/
                for(j=0;j<len;j++){
                    /*Flip LSB if current code bit is 1*/
                    if(comp&code){
                        bitBuff = bitBuff+1;
                    }
                    /*Shift mask to next bit*/
                    comp = comp>>1;
                    /*shift buffer and track number of bits written*/
                    buffCount++;
                    /*If buffer is full, write to file*/
                    if(buffCount == 8){
                        if(write(fdout,&bitBuff,1) < 0){
                            perror("Bitbuff");
                            exit(EXIT_FAILURE);
                        }
                        bitBuff = 0;
                        buffCount = 0;
                    }
                    else{
                        bitBuff = bitBuff << 1;
                    }
                }
            }
        }
    }
    if(numRead == -1){
        perror("Freq Read");
        exit(EXIT_FAILURE);
    }
    /*If buffer is not empty*/
    if(buffCount > 0){
        /*Pad buffer with 0*/
        while(buffCount < 7){
            bitBuff = bitBuff << 1;
            buffCount++;
        }
        /*Write last byte to file*/
        if(write(fdout,&bitBuff,1) < 0){
            perror("Bitbuff");
            exit(EXIT_FAILURE);
        }
        bitBuff = 0;
        buffCount = 0;
    }
}

/*Parse Header into Histogram*/
int* parseHeader(int fdin){
    /*Initialize freqs*/
    int* freqs = (int*)malloc(256*sizeof(int));
    int i;
    for(i = 0; i<256;i++){
        freqs[i] = 0;
    }

    /*Read unique count from 1st byte*/
    /*lseek(fdin,0,SEEK_SET);*/
    uint8_t ucount = 0;
    if(read(fdin, &ucount, 1)==-1){
        perror("Read Count Byte");
        exit(EXIT_FAILURE);
    }
    ucount++;

    /*Read entire header*/
    int headerSize = ucount*5;
    uint8_t buff[headerSize];
    if(read(fdin, &buff, headerSize)==-1){
        perror("Read Count Byte");
        exit(EXIT_FAILURE);
    }
    /*For num uniq chars*/
    uint8_t index = 0;
    int count = 0;
    for(i=0;i<headerSize-4;i = i+5){
        /*Read 1 byte for index*/
        index = buff[i];
        /*Read 4 bytes for count*/
        count = (buff[i+1]<< 24) | (buff[i+2]<< 16) | (buff[i+3]<< 8) |
        (buff[i+4]);
        /*Assign count to index of freqs*/
        freqs[index] = count;
    }
    return freqs;
}

/*Sum freqs*/
int sumFreqs(int* freqs){
    int sum = 0;
    int i;
    for(i = 0; i<256;i++){
        sum = sum + freqs[i];
    }
    return sum;
}

/*Sum freqs*/
int isOneChar(int* freqs){
    int ind = 0;
    int found = 0;
    int i;
    /*Return index if one char, -1 if not*/
    for(i = 0; i<256;i++){
        if(freqs[i] != 0){
            if(found == 0){
                found = 1;
                ind = i;
            }
            else{
                return -1;
            }
        }
    }
    return ind;
}

/*Decode Body of file*/
void decodeBody(int fdin, int fdout, codeIndex* codeInd, int total){
    /*Read in buffer*/
    char readBuff[BUFF_SIZE];
    char writeBuff[BUFF_SIZE];
    int writeBuffCount = 0, bytesWritten = 0;
    int code = 0, len = 0, numRead = 0;
    uint8_t byte = 0, bitMask = 0;
    int i,j,k;
    while((numRead = read(fdin, &readBuff, BUFF_SIZE)) > 0){
        /*Read buffer byte by byte*/
        for(i=0;(i<numRead) & (bytesWritten < total);i++){
            byte = readBuff[i];
            /*deconstruct byte bit by bit*/
            bitMask = 128;
            for(j=0;(j<8) & (bytesWritten < total);j++){
                /*Add bit to temp code*/
                if(byte & bitMask){
                    code = code + 1;
                }
                bitMask = bitMask/2;
                len++;

                /*check temp code and length against code index*/
                for(k=0;k<256;k++){
                    if((code == codeInd->codes[k])&&(len == codeInd->lens[k])){
                            /*if code is found reset temp code and len*/
                            code = 0;
                            len = 0;
                            /*Write char to buffer*/
                            writeBuff[writeBuffCount] = k;
                            writeBuffCount++;
                            bytesWritten++;
                            /*If buffer is full, write it to file*/
                            if(writeBuffCount >= BUFF_SIZE){
                                if(write(fdout,&writeBuff, BUFF_SIZE) < 0){
                                    perror("writeBuff");
                                    exit(EXIT_FAILURE);
                                }
                                writeBuffCount = 0;
                            }
                            break;
                    }
                }
                code = code*2;
            }
        }

    }
    /*Write Remaining buffer*/
    if(write(fdout,&writeBuff, writeBuffCount) < 0){
        perror("Final writeBuff");
        exit(EXIT_FAILURE);
    }
}

void writeSingleChar(int fdout, int unique, int num){
    char writeBuff[num];
    int i;
    for(i=0;i<num;i++){
        writeBuff[i] = unique;
    }
    if(write(fdout,&writeBuff, num) < 0){
        perror("Final writeBuff");
        exit(EXIT_FAILURE);
    }
}
