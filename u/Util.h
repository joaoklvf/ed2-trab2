#ifndef TRABALHO_H   
#define TRABALHO_H

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define HEADER_ADDED_COUNTER_INDEX 0
#define HEADER_REMOVED_COUNTER_INDEX 4
#define HEADER_LAST_REMOVED_INDEX 8
#define HEADER_INITIAL_REGISTER_POSITION 12

#define OFFSETS_HEADER_IS_SORTED_INDEX 0
#define OFFSETS_HEADER_INITIAL_REGISTER_POSITION 1
#define OFFSET_ISNT_ORGANIZED_CHAR "#"
#define OFFSET_IS_ORGANIZED_CHAR "*"

struct record {
    char cod_cli[12];
    char cod_vei[8];
    char client[50];
    char veiculo[50];
    char dias[4];
};

struct searchKey {
    char cod_cli[12];
    char cod_vei[8];
};

struct searchKeyRecord {
    char cod_cli[12];
    char cod_vei[8];
    int offset;
};

struct header{
	int addedCounter;
	int searchedCounter;
	int lastAdded;
};

struct loadedFileDto {
    record records[25];
    searchKey keys[25];
};

int pega_campo(char *p_registro, int *p_pos, char *p_campo);
int pega_registro(FILE *p_out, char *p_reg);
int readCreateFiles(loadedFileDto *retorno, FILE *mainFile, searchKeyRecord *keysRecords);
int inserir(loadedFileDto *loadedFiles, searchKeyRecord *searchKeyRecordToAdd, FILE *mainFile);
int search(loadedFileDto *loadedFiles, FILE *mainFile);
int getHeaderData(FILE *mainFile, int index);
FILE* openFile(FILE *mainFile);
int incrementHeaderCounter(FILE *mainFile, int index, int increment);
void mergeSortByCities(searchKeyRecord *arr, int l, int r);
void mergeByCities(searchKeyRecord *arr, int l, int m, int r);
void printArray(searchKeyRecord *L, int size);
int writeOffsetFile(searchKeyRecord *searchKeyRecords);

#endif
