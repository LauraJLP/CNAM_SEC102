/**
* \file dirutil.c
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 List files in a directory
*
* Library functions used by the main application.
*/

#include <stdio.h>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

#include "dirutil.h"

/**
* \brief append the string given to the directory name given, the data will be copied to the buffer indicated
*
* \param _In_	dir			input directory to copy to destDir
* \param _In_	strToAdd	string to append to the destDir
* \param _Out_	destDir		buffer containing <dir>\\<destDir>
*/
unsigned int writeNewDir
(
	char * const dir,			/* input directory */
	char * strToAdd,	/* string to append to dir name*/
	char * destDir		/* output directory */
)
{
	size_t lenDir = 0;

	// Check length of dir path , it must not exceed 
	// MAX_PATH - strlen(strToAdd)

	if (StringCchLength(dir, MAX_PATH, &lenDir) != ERROR_SUCCESS)
	{
		printf("DIR_UTIL_ERROR directory path %s is too long, it should not exceed %d characters\n", dir, MAX_PATH);
		return ERROR_LABEL_TOO_LONG;
	}
	
	if (lenDir > (MAX_PATH - strlen(strToAdd)))
	{
		printf("DIR_UTIL_ERROR directory path %s is too long, it should not exceed %d characters\n", dir, MAX_PATH);
		return ERROR_LABEL_TOO_LONG;
	}

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopy(destDir, MAX_PATH, dir);
	StringCchCat(destDir, MAX_PATH, TEXT(strToAdd));
	return ERROR_SUCCESS;
}

/**
* \brief list the contents o the directory given
*
* \param _In_ dir	directory whose contents are to be displayed
* \return ERROR_SUCCESS or error code indicating any error encountered
*/
unsigned int listDir(char * const dir) 
{
	WIN32_FIND_DATA		ffd;
	LPWIN32_FIND_DATA	pFfd = &ffd;
	char				destDir[MAX_PATH];
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	LARGE_INTEGER		filesize;
	SYSTEMTIME			sysTime;
	LPSYSTEMTIME		pSysTime = &sysTime;

	if (writeNewDir(dir, "\\*", &destDir) != ERROR_SUCCESS)
	{
		return ERROR_LABEL_TOO_LONG;
	}
	// Find the first file in the directory.

	hFind = FindFirstFile(destDir, pFfd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		printf("FindFirstFile ERROR!\n");
		return ERROR_LABEL_TOO_LONG;
	}
	printf("Name                    Dir  Size       Created          Last Modified    Last Accessed    Hidden  System  Read Only\n");
	printf("----                    ---  ----       -------          -------------    -------------    ------  ------  ---------\n");

	while (FindNextFile(hFind, pFfd) != 0)
	{
		// this is a directory 

		if (pFfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			printf("%-23.23s yes  0          ", pFfd->cFileName);
		}
		else
		{
			filesize.LowPart = pFfd->nFileSizeLow;
			filesize.HighPart = pFfd->nFileSizeHigh;
			printf("%-23.23s no   %10.10ld ", pFfd->cFileName, (long)filesize.QuadPart);
		}
		FileTimeToSystemTime(&pFfd->ftCreationTime, pSysTime);
		printf("%2d/%2d/%d:%2d.%2d ",
			pSysTime->wDay, pSysTime->wMonth, pSysTime->wYear,
			pSysTime->wHour, pSysTime->wMinute);
		FileTimeToSystemTime(&pFfd->ftLastAccessTime, pSysTime);
		printf("%2d/%2d/%d:%2d.%2d ",
			pSysTime->wDay, pSysTime->wMonth, pSysTime->wYear,
			pSysTime->wHour, pSysTime->wMinute);
		FileTimeToSystemTime(&pFfd->ftLastWriteTime, pSysTime);
		printf("%2d/%2d/%d:%2d.%2d ",
			pSysTime->wDay, pSysTime->wMonth, pSysTime->wYear,
			pSysTime->wHour, pSysTime->wMinute);
		/* hidden, system, read only*/
		if (pFfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			printf("yes     ");
		else
			printf("no      ");
		if (pFfd->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
			printf("yes     ");
		else
			printf("no      ");
		if (pFfd->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			printf("yes     \n");
		else
			printf("no      \n");
	}
	FindClose(hFind);
	return ERROR_SUCCESS;
}
