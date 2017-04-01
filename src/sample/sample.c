#include <Windows.h>
#include <stdio.h>

#include "sample.h"

unsigned int main(int argc, char* argv[])
{

    unsigned int Code = ERROR_GEN_FAILURE;

    if (argc != 2){
    
        if (ERROR_SUCCESS != Usage(argv[0])) {
            Code = ERROR_INTERNAL_ERROR;
        }

        Code = ERROR_INVALID_PARAMETER;
    }

    Code = ERROR_SUCCESS;

    return Code;
}

unsigned int Usage(char* lpszProgramName) {

    printf("Usage : %s foo", lpszProgramName);
    return ERROR_SUCCESS;

}