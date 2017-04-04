/**
* \file proclist.c
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 Process/Module enumeration
*
* Usage: proclist 
* 
* This program will list all processes and for each process found, list the path to its
* executable, the list of modules loaded and the size and load address for the modules.
* After each process located the user should hit carriage return to continue program execution
* this is to enable the user to read the output at his ease.
*/

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

#include "proclist.h"

/**
* \brief print process name, executable, list of loaded modules and address and size for the modules
*
* \param _In_ processID id of the process whose data is to be displayed
*/

void printProcessInfo(DWORD processID)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	TCHAR imageFileName[MAX_PATH];
	HANDLE hProcess;
	unsigned int i;

	// Get a handle to the process.

	 hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	// Get the process name.

	if (NULL != hProcess)
	{
		DWORD cbNeeded;
		HMODULE * hModList;
		DWORD initialSize = sizeof(HMODULE) * 4096;

		if ((hModList = (HMODULE *)malloc(initialSize)) == NULL)
		{
			printf("\tError allocating memory for module list\n");
			return;
		}
		if (EnumProcessModulesEx(hProcess, hModList, initialSize,
			&cbNeeded, LIST_MODULES_ALL))
		{
			/* if we hadn't allocated enough space to hold all the module info
			    reallocate here
				*/
			if (cbNeeded > initialSize) {
				if (realloc(hModList, cbNeeded) == NULL) {
					printf("\tError allocating memory for module list\n");
					free(hModList);
					return;
				}
			}
			for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
			{
				TCHAR szModName[MAX_PATH];
				MODULEINFO * modInfo;

				GetModuleBaseName(hProcess, hModList[i], szProcessName,
					sizeof(szProcessName) / sizeof(TCHAR));

				// Get the full path to the module's file.

				if (GetModuleFileNameEx(hProcess, hModList[i], szModName,
					sizeof(szModName) / sizeof(TCHAR)))
				{
					// Print the module name and handle value.

					printf("\t%s (0x%08X)\n", szModName, hModList[i]);
				}
				if ((modInfo = (MODULEINFO *)malloc(sizeof(MODULEINFO))) == NULL)
					printf("\tCouldn't allocate memory for module info structure!\n");
				else {
					if ((GetModuleInformation(hProcess, hModList[i], modInfo, sizeof(MODULEINFO)))) {
						printf("\tModule load address: 0x%08X size: %d bytes\n", modInfo->lpBaseOfDll, modInfo->SizeOfImage);
					}
					free(modInfo);
				}
			}
		}
		// free the memory allocated for the modules
		free(hModList);
	}

	// Print the process name and identifier followed by the path to the executable

	printf("%s  (PID: %u)\n", szProcessName, processID);
	if (GetProcessImageFileName(hProcess, imageFileName, sizeof(imageFileName)) > 0)
		printf("%s  \n", imageFileName);

	// Release the handle to the process.

	CloseHandle(hProcess);
}

/**
* \brief main entry point for the application - no args required
*/
int main(void)
{
	// Get the list of process identifiers.
	DWORD				aProcesses[1024], cbNeeded, cProcesses;
	unsigned int		i;
	char				ch;
	DWORD				read;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return 1;

	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for (i = 0; i < cProcesses; i++) {
		if (aProcesses[i] != 0)
			printProcessInfo(aProcesses[i]);
		printf("\n---------------Hit enter to continue------------------\n\n");
		ReadConsole(GetStdHandle(STD_INPUT_HANDLE), &ch, 1, &read, NULL);
	}
	return 0;
}