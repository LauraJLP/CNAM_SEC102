/**
* \file regutil.h
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 ROT13 / Registry assignment
*
* Header file for library functions to read the Windows registry.
*/


/*
* see https://www.sepago.com/blog/2010/07/20/how-long-can-a-registry-key-name-really-be for
* an explanation of why I am defining the buffer to be 2 more bytes longer than MAX_PATH
*/
#define BUF_SIZE 257

struct strLinkedList {
	char * str;
	struct strLinkedList * next;
	struct strLinkedList * prev;
};

struct keyData {
	char * keyName;
	struct strLinkedList * keyEnums;
	struct keyData * next;
};

void freeDataFromRegistry(struct keyData * pRegList);
unsigned int readDataFromRegistry(char * key, char * subkey, struct keyData ** ppKeyData);




