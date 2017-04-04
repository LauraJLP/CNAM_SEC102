/**
* \file rot13.h
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 ROT13 / Registry assignment
*
* Header file for main application
*/

void usage(char* lpszProgramName);
unsigned int parseArgs(int argc, char * const argv[], char ** returnStr, int * isFile);
unsigned int main(int argc, char * const argv[]);
