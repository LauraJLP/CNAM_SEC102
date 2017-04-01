#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

#include "proclist.h"

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void PrintProcessNameAndID(DWORD processID)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	TCHAR imageFileName[MAX_PATH];
	HANDLE hProcess;
	DWORD cbNeeded;
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
		int initialSize = sizeof(HMODULE) * 4096;

		if ((hModList = (HMODULE *)malloc(initialSize)) == NULL)
		{
			_tprintf(TEXT("\tError allocating memory for module list\n"));
			return;
		}
		if (EnumProcessModulesEx(hProcess, hModList, initialSize,
			&cbNeeded, LIST_MODULES_ALL))
		{
			/* if we hadn't allocated enough space to hold all the module info
			    reallocate here
				*/
			_tprintf(TEXT("DEBUG cbNeeded: %d sizeof HMODULE* %d\n", cbNeeded, sizeof(HMODULE)));
			if (cbNeeded > initialSize) {
				if (realloc(hModList, cbNeeded) == NULL) {
					_tprintf(TEXT("\tError allocating memory for module list\n"));
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

					_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hModList[i]);
				}
				if ((modInfo = (MODULEINFO *)malloc(sizeof(MODULEINFO))) == NULL)
					_tprintf(TEXT("\tCouldn't allocate memory for module info structure!\n"));
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

	_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);
	if (GetProcessImageFileName(hProcess, imageFileName, sizeof(imageFileName)) > 0)
		_tprintf(TEXT("%s  \n"), imageFileName);

	// Release the handle to the process.

	CloseHandle(hProcess);
}

int main(void)
{
	// Get the list of process identifiers.

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 1;
	}


	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			PrintProcessNameAndID(aProcesses[i]);
		}
	}

	return 0;
}