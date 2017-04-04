/**
* \file rotutil.c
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 ROT13 / Registry assignment
*
* Library of functions to perform ROT13 encryption and to write contents to file or data buffer.
*/

#include <Windows.h>
#include <stdio.h>

#include "rotutil.h"

DWORD g_BytesTransferred = 0;

/**
* \brief Callback routine when file read has completed.
* \param _In_ dwErrorCode error code
* \param _In_ dwNumberOfBytesTransfered number of bytes read from file
* \param _In_ lpOverlapped overlapped value - not used
*/
VOID CALLBACK fileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped)
{
	g_BytesTransferred = dwNumberOfBytesTransfered;
}

/**
* \brief Create a new filename from the filename given but with the rot13 extension
*
* This routine will allocate a string to store the name of a new file, the name will be identical
* to the filename passed in <filename> but the extension will be changed to .rot13.
* \param _In_	filename		name of file name to use
* \param _Out_	pRotFilename	new filename, will be <filename>.rot13
* \return ERROR_SUCCESS or error code indicating the reason for the program failure
*/
unsigned int getRotFileName(char * filename, char ** pRotFilename) {
	char * ptr;
	int i = 0;

	if ((*pRotFilename = (char*)malloc(strlen(filename) + 3)) == NULL) {
		return ERROR_NOT_ENOUGH_MEMORY;
    }
	ptr = *pRotFilename;
	memset(ptr, '\0', sizeof(ptr));
	
	while (filename[i] != '\.') {
		ptr[i] = filename[i];
		i += 1;
	}
	ptr[i] = '.'; i += 1;
	ptr[i] = 'r'; i += 1;
	ptr[i] = 'o'; i += 1;
	ptr[i] = 't'; i += 1;
	ptr[i] = '1'; i += 1;
	ptr[i] = '3'; i += 1;
	ptr[i] = '\0';
	return ERROR_SUCCESS;
}

/**
* \brief Open the filename given and encrypt all strings in the file using ROT13 encryption
*
* This routine will open the file given, it will read all the data in the file, encode all the data
* using the ROT13 algorithm and then write the data into a file with the same name as the input file
* but with the extension .rot13.
*
* \param _In_	filename		name of file name to open containing the strings to encrypt
* \return ERROR_SUCCESS or error code indicating the reason for the program failure
*/
unsigned int encodeRot13FromFile(char * filename) {
	HANDLE			hFile;
	int				retErr = ERROR_SUCCESS;
	DWORD			dwBytesRead = 0;
	char			buf[BUFFERSIZE] = { 0 };
	OVERLAPPED		ol = { 0 };
	char *			result;
	char *			rotFilename;
	DWORD			dwBytesWritten = 0;
	DWORD			dwBytesToWrite;

	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("File %s doesn't exist!\n", filename);
		retErr = ERROR_FILE_NOT_FOUND;
		return retErr;
	}
	// Read one character less than the buffer size to save room for
	// the terminating NULL character. 

	if (FALSE == ReadFileEx(hFile, buf, BUFFERSIZE - 1, &ol, fileIOCompletionRoutine))
	{
		printf("encodeRot13FromFile error : terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
		CloseHandle(hFile);
		return -1;
	}
	SleepEx(5000, TRUE);
	dwBytesRead = g_BytesTransferred;

	// read the data - assuming it is ANSI

	if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE - 1)
		buf[dwBytesRead] = '\0'; // add end of string NULL character
	else if (dwBytesRead == 0) {
		printf("encodeRot13FromFile error : No data read from file %s\n", filename);
		return -1;
	} 
	else {
		printf("encodeRot13FromFile error : Unexpected value for dwBytesRead\n");
		return -1;
	}
	// now encode the data
	encodeRot13(buf, &result);
	
	// now create a new file name - same as input file but with .rot13 extension
	getRotFileName(filename, &rotFilename);

	// now write the encoded data to a file called filename.rot13
	hFile = CreateFile(rotFilename, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,	NULL); 
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("encodeRot13FromFile error : Unable to open file %s for write.\n", rotFilename);
		return -1;
	}
	
	dwBytesToWrite = strlen(result);
	if (WriteFile(hFile, result, dwBytesToWrite, &dwBytesWritten, NULL) == -1) {
		printf("encodeRot13FromFile error : Unable to write to file %s.\n", rotFilename);
	}
	else {
		if (dwBytesWritten != dwBytesToWrite) {
			// This is an error because a synchronous write that results in
			// success (WriteFile returns TRUE) should write all data as
			// requested. This would not necessarily be the case for
			// asynchronous writes.
			printf("encodeRot13FromFile error: dwBytesWritten != dwBytesToWrite\n");
		}
		else {
			printf("Data written to file: %s successfully!\n", rotFilename);
		}
	}

	// close the open file handles 

	CloseHandle(hFile);
	return retErr;
}

/**
* \brief Encode the string parsed using ROT13 alorithm and write result to string passed
*
* This routine will allocate a string the size of the input string. It will then encrypt the input
* string using the ROT13 algorithm and copy the result to the result string.
*
* \param _In_	strToEncode		string to encode using ROT13 algorithm
* \param _Out_	result			value of input string encoded as ROT13
* \return ERROR_SUCCESS or error code indicating the reason for the program failure
*/
unsigned int encodeRot13(char * strToEncode, char ** result) {
	int		key, length;
	LPTSTR	locStr = strToEncode;
	char *	pStr;

	/* we will begin by capitalizing the string */
	CharUpper(locStr);
	length = strlen(locStr) + 1;

	/* allocate the memory to store the converted string */
	if ((*result = (char *)malloc(length * sizeof(char))) == NULL) {
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	memset(*result, '\0', sizeof(*result));
	pStr = *result;
	key = 13;
	/* 
	 * loop through the string and add 13 to/from each character - if the value
	 * exceeds 26 then perform a modulo 2 to calculate the correct character
	 * if the character is not in the A-Z character set then simply skip - no
	 * conversion is made
	 */ 
	for (int i = 0; i < length; i++)
	{
		int currentLetter = locStr[i];
		char cipher = currentLetter + key;

		/* 
		 * first ensure the character is in the A-Z range, if it isn't
		 * simply ignore it
		 */
		if (currentLetter > 64 && currentLetter < 91) {
			if ((currentLetter - 'A') + key >= 26)
			{
				key = ((currentLetter - 'A') + key) % 26;
				cipher = 'A' + key;
			}

			pStr[i] = (char)cipher;
			/* reset the key and do the next letter */
			key = 13;
		}
		else {
			pStr[i] = locStr[i];
		}
	}
	pStr[length-1] = '\0';
	return length;
}

