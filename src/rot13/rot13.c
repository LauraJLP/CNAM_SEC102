/**
* \file rot13.c
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 ROT13 / Registry assignment
*
* Usage: rot13 [-f <filename>|stringToEncode]
*
* -f <filename> :	indicate the path to a text file containing several strings to be encoded using ROT13
*					encryption, the result will be written to a file in the same location but with the 
*					extension .rot13
* 
* stringToEncode :	simply parse a simple string to the program, it will encode and then decode it using
*					ROT13 encryption and display the results on the console
*
* Once the initial string or file content ROT13 encryption has completed, the application will then read
* the keys in the Windows Registry located at :
*	HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\UserAssist 
* for each key located, the subkey "Count" will be appended and the registry queried for all hives located at :
*	HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\UserAssist\<KeyFound>\Count
* All data will be stored in two different linked list structures and displayed to the console. The quantity of
* data displayed maybe very large and so for each set of data displayed the user must hit any key to continue
* program execution.
*/

#include <Windows.h>
#include <Shlwapi.h>
#include <stdio.h>

#include "regutil.h"
#include "rot13.h"
#include "rotutil.h"


/**
* \brief Parse the command line arguments.
* \param _In_ argc: Number of parameters passed to the program
* \param _In_ argv: List of all parameters of type char * const
* \param _Out_ returnStr: chain of characters read from the command line or name of file to read
* \param _Out_ isFile: set to 1 if the return string contains a filename or to 0 if it is simply a string to ROT13 encode
* \return ERROR_SUCCESS or error code indicating the reason for the program failure
* \remark
*/
unsigned int parseArgs(int argc, char * const argv[], char ** returnStr, int * isFile) {
	int				retErr = ERROR_SUCCESS;
	WIN32_FIND_DATA FindFileData;
	HANDLE			handle;
	int				found;

	if (argc == 3) {
		if (strncmp(argv[1], "-f", 2) == 0) {
			*returnStr = argv[2];
			*isFile = 1;
			/* check file exists */
			handle = FindFirstFile(*returnStr, &FindFileData);
			found = handle != INVALID_HANDLE_VALUE;
			if (!found) {
				printf("File %s doesn't exist!\n", *returnStr);
				usage(argv[0]);
				retErr = ERROR_FILE_NOT_FOUND;
			}
			FindClose(handle);
			return retErr;
		}
		else {
			printf("Error arg:%s not supported!\n", argv[1]);
			usage(argv[0]);
			retErr = ERROR_INVALID_PARAMETER;
			return retErr;
		}
	}
	if (argc == 2) {
		if (strncmp (argv[1], "-f", 2) != 0) {
			*isFile = 0;
			*returnStr = argv[1];
		}
		else {
			printf("Error -f must be followed by a filename to read!\n");
			usage(argv[0]);
			retErr = ERROR_INVALID_PARAMETER;
		}
	}
	else {
		usage(argv[0]);
		retErr = ERROR_INVALID_PARAMETER;
	}
	return retErr;
}

/**
* \brief Print out a program usage message.
* \param _In_ lpszProgramName: Name of the program
* \remark
*/
void usage(char* lpszProgramName) {

	printf("Usage : %s -f <file containing ROT strings> [string to convert to ROT13]", lpszProgramName);
}

/**
 * \brief This is the main entry point.
 * \param _In_ argc: Number of parameters passed to the program
 * \param _In_ argv: List of all parameters of type char
 * \return ERROR_SUCCESS or error code indicating the reason for the program failure
 * \remark
 */
unsigned int main(int argc, char * const argv[])
{
	char *					decodeStr;
	char *					encodeStr;
	int						len = 0;
	unsigned int			retCode = ERROR_GEN_FAILURE;
	char *					retStr = NULL;
	int						isFile = 0;
	struct keyData *		pRegList;
	struct keyData *		pWorking;
	struct strLinkedList *	pWorkingLl;
	char					ch;
	DWORD					read;


	retCode = parseArgs(argc, argv, &retStr, &isFile);
	if (retCode != ERROR_SUCCESS)
		return retCode;
	
	if (!isFile) {
		len = strlen(argv[0]);
		printf("String parsed: %s\n", argv[1]);
		encodeRot13(argv[1], &encodeStr);
		printf("String encoded: %s\n", encodeStr);
		encodeRot13(encodeStr, &decodeStr);
		printf("String decoded: %s\n", decodeStr);
		free(encodeStr);
		free(decodeStr);
	}
	else {
		encodeRot13FromFile(retStr);
	}
	printf("---------------Hit enter to continue------------------\n");
	ReadConsole(GetStdHandle(STD_INPUT_HANDLE), &ch, 1, &read, NULL);

	retCode = readDataFromRegistry("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist", "Count", &pRegList);
	if (retCode != ERROR_SUCCESS)
		return retCode;
	pWorking = pRegList;
	while (pWorking != NULL && pWorking->keyName != NULL) {
		printf("Registry string name: %s\n", pWorking->keyName);
		pWorkingLl = pWorking->keyEnums;
		while (pWorkingLl->str != NULL) {
			printf("\tRegistry string val: %s\n", pWorkingLl->str);
			encodeRot13(pWorkingLl->str, &decodeStr);
			printf("\tRegistry string decoded: %s\n", decodeStr);
			free(decodeStr);
			pWorkingLl = pWorkingLl->next;
		}
		pWorking = pWorking->next;
		printf("\n---------------Hit enter to continue------------------\n\n");
		ReadConsole(GetStdHandle(STD_INPUT_HANDLE), &ch, 1, &read, NULL);
	}
	/* now let's free up the memory allocated */
	freeDataFromRegistry(pRegList);
	retCode = ERROR_SUCCESS;

    return retCode;
}
