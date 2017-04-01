#include <Windows.h>
#include <stdio.h>

#include "dirlist.h"
#include "dirutil.h"

unsigned int main(int argc, char* argv[])
{
    unsigned int errCode = ERROR_GEN_FAILURE;
	char * dir;

    if (argc != 2){
    
        if (ERROR_SUCCESS != usage(argv[0])) {
            errCode = ERROR_INTERNAL_ERROR;
        }
        errCode = ERROR_INVALID_PARAMETER;
	}
	else {
		dir = argv[1];
		errCode = listDir(dir);
	}

    return errCode;
}

unsigned int usage(char* lpszProgramName) {

    printf("Usage : %s <dirName>", lpszProgramName);
    return ERROR_SUCCESS;

}