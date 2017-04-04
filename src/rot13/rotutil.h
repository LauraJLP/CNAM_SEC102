/**
* \file rotutil.h
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 ROT13 / Registry assignment
*
* Header file for library of functions to perform ROT13 encryption and to write contents to file or data buffer.
*/
#define BUFFERSIZE 256

unsigned int encodeRot13(char * strToEncode, char ** result);
unsigned int encodeRot13FromFile(char * filename);
