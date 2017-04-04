/**
* \file dirlist.c
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 List files in a directory
*
* Usage: dirlist <directory>
*
* <directory> :	indicates the directory whose contents you wish to display
*/

#include <Windows.h>
#include <stdio.h>

#include "dirlist.h"
#include "dirutil.h"

/**
* \brief This is the main entry point.
* \param _In_ argc: Number of parameters passed to the program
* \param _In_ argv: List of all parameters of type char
* \return ERROR_SUCCESS or error code indicating the reason for the program failure
* \remark
*/
unsigned int main(int argc, char* const argv[])
{
    unsigned int errCode = ERROR_GEN_FAILURE;

    if (argc != 2){
        if (ERROR_SUCCESS != usage(argv[0])) {
            errCode = ERROR_INTERNAL_ERROR;
        }
        errCode = ERROR_INVALID_PARAMETER;
	}
	else {
		errCode = listDir(argv[1]);
	}

    return errCode;
}

/**
* \brief print program usage message
*
* \param _In_ lpszProgramName name of this application
* \return ERROR_SUCCESS
*/
unsigned int usage(char* lpszProgramName) {
    printf("Usage : %s <dirName>", lpszProgramName);
    return ERROR_SUCCESS;
}