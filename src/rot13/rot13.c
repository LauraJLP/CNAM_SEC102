#include <Windows.h>
#include <stdio.h>

#include "rot13.h"
#include "rotutil.h"

/*
 * \brief This is the main entry point.
 * \param _In_ argc: Number of parameters passed to the program
 * \param _In_ argv: List of all parameters of type char
 * \return ERROR_SUCCESS or error code indicating the reason for the program failure
 * \remark
 */
unsigned int main(int argc, char* argv[])
{
	char *			result;
	int				len = 0;
	unsigned int	Code = ERROR_GEN_FAILURE;

    if (argc != 2){
        if (ERROR_SUCCESS != Usage(argv[0])) {
            Code = ERROR_INTERNAL_ERROR;
        }
        Code = ERROR_INVALID_PARAMETER;
    }
	len = strlen(argv[0]);
	if ((result = (char *)malloc(len * sizeof(char))) == NULL) {
		printf("Error allocating memory for return string!\n");
		Code = ERROR_NOT_ENOUGH_MEMORY;
	}
	else {
		printf("String parsed: %s\n", argv[1]);
		encodeRot13(argv[1], result);
		printf("String encoded: %s\n", result);
		encodeRot13(result, result);
		printf("String decoded: %s\n", result);

	}
    Code = ERROR_SUCCESS;

    return Code;
}

unsigned int Usage(char* lpszProgramName) {

    printf("Usage : %s <string to convert to ROT13>", lpszProgramName);
    return ERROR_SUCCESS;

}
