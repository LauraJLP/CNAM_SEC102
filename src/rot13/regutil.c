/**
* \file regutil.c
* \author Laura Le Padellec
* \date 4 April 2017
* \brief Implementation of CNAM SEC102 ROT13 / Registry assignment
*
* Library function to read the Windows registry.
*/

#include <Windows.h>
#include <stdio.h>

#include "regutil.h"

/**
 * \brief free data allocated by the routine readDataFromRegistry()
 *
 * This procedure will work through the linked list pointed to by
 * <pRegList> and will free all memory in the linked list.
 *
 * \param _In_ pRegList a pointer to the linked list to be freed
 *
*/
void freeDataFromRegistry(struct keyData * pRegList) {
	struct keyData *		pWorking;
	struct keyData *		pWorkingTmp;
	struct strLinkedList *	pList;
	struct strLinkedList *  pListTmp;
	int i = 0;
	
	pWorking = pRegList;
	
	while (1) {
		i += 1;
		if (pWorking->keyName != NULL) {
			free(pWorking->keyName);
		}
		pList = pWorking->keyEnums;
		while (pList != NULL) {
			pListTmp = pList;
			if (pList->str != NULL) {
				free(pList->str);
				pList->str = NULL;
			}
			if (pList->next != NULL) 
				pList = pList->next;
			else 
				break;
		}
		free(pWorking->keyEnums);
		pWorking->keyEnums = NULL;
		pWorkingTmp = pWorking;
		if (pWorking->next != NULL)
			pWorking = pWorking->next;
		else 
			break;
	}
	free(pWorkingTmp);
}

/**
 * \brief read data from the registry with the path given and the subkey given
 *
 * This procedure will read data from the HKEY_CURRENT_USER registry key first using
 * the subkey specified in the parameter key. It will then enumerate all keys below this
 * subkey and for each key found read the concatenated key given by
 * key\<keyFound>\subkey. For each value found a new strLinkedList structure will be
 * allocated and added to the strLinkedList header pointer. Be sure to call
 * freeDataFromRegistry() to free up all the memory allocated once it is no longer
 * required.
 *
 * \param _In_	key			the first key to be accessed
 * \param _In_	subkey		the second key to be appended to the first key + the key located
 * \param _Out_ ppKeyData	pointer to a pointer of keyData structures containing all data read
 * \return	ERROR_SUCCESS	or an error code indicating the error encountered
 */
unsigned int readDataFromRegistry(char * key, char* subkey, struct keyData ** ppKeyData) {
	HKEY					hKey;
	PHKEY					phkResult;
	HKEY					hSubKey;
	PHKEY					phSubKeyResult;

	unsigned int			retCode;
	char					keyName[BUF_SIZE];
	unsigned int			keyLen = BUF_SIZE;
	char					subKeyName[BUF_SIZE];
	char					keyBuf[BUF_SIZE];
	int						i = 0;
	char *					pKeyBuf = NULL;

	struct strLinkedList *	pLinkedList = NULL;
	struct keyData *		pKeyData = NULL;

	if ((*ppKeyData = (struct keyData *)malloc(sizeof(struct keyData))) == NULL) {
		printf("regutil:readDataFromRegistry memory allocation error\n");
		retCode = ERROR_NOT_ENOUGH_MEMORY;
	}
	
	pKeyData = *ppKeyData;
	phkResult = &hKey;
	phSubKeyResult = &hSubKey;

	if ((retCode = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_ALL_ACCESS, phkResult)) != ERROR_SUCCESS) {
		printf("regutil:readDataFromRegistry error opening registry key %s\n", key);
		free(*ppKeyData);
		return retCode;
	}

	/* now enumerate the keys found and for each key extract its value*/
	
	while (1) {
		keyLen = BUF_SIZE;
		retCode = RegEnumKeyEx(hKey, i, keyName, &keyLen, NULL, NULL, NULL, NULL);
		if (retCode != ERROR_SUCCESS)
			break;
#ifdef DEBUG
		printf("regutil:readDataFromRegistry debug key name returned: %s\n", keyName);
#endif
		strcpy(subKeyName, key);
		strcat(subKeyName, "\\");
		strcat(subKeyName, keyName);
		strcat(subKeyName, "\\");
		strcat(subKeyName, subkey);
		retCode = RegOpenKeyEx(HKEY_CURRENT_USER, subKeyName, 0, KEY_ALL_ACCESS, phSubKeyResult);
		if (retCode != ERROR_SUCCESS)
			printf("regutil:readDataFromRegistry error opening registry key %s\n", subKeyName);

		if ((pKeyBuf = (char *)malloc(strlen(subKeyName) + 1)) == NULL) {
			printf("regutil:readDataFromRegistry memory allocation error\n");
			retCode = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}
		memset(pKeyBuf, '\0', sizeof(pKeyBuf));
		strcpy(pKeyBuf, subKeyName);
		pKeyData->keyName = pKeyBuf;
		// create a linked list structure to save all hives for this key
		if ((pLinkedList = (struct strLinkedList *)malloc(sizeof(struct strLinkedList))) == NULL) {
			printf("regutil:readDataFromRegistry memory allocation error\n");
			break;
		}
		pKeyData->keyEnums = pLinkedList;
		// init all fields in the first element in the list
		pLinkedList->str = NULL;
		pLinkedList->next = NULL;
		pLinkedList->prev = NULL;
		keyLen = BUF_SIZE;
		int j = 0;
		// enumerate all the hives for the subkey found adding each value to a linked list
		while ((retCode = RegEnumValue(hSubKey, j, keyBuf, &keyLen, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS) {
			if ((pKeyBuf = (char *)malloc(keyLen + 1)) == NULL) {
				printf("regutil:readDataFromRegistry memory allocation error\n");
				retCode = ERROR_NOT_ENOUGH_MEMORY;
				break;
			}
			memset(pKeyBuf, '\0', sizeof(pKeyBuf));
			strcpy(pKeyBuf, keyBuf);
			pLinkedList->str = pKeyBuf;
			if ((pLinkedList->next = (struct strLinkedList *)malloc(sizeof(struct strLinkedList))) == NULL) {
				printf("regutil:readDataFromRegistry memory allocation error\n");
				retCode = ERROR_NOT_ENOUGH_MEMORY;
				break;
			}
			pLinkedList = pLinkedList->next;
			pLinkedList->str = NULL;
			pLinkedList->next = NULL;
			keyLen = BUF_SIZE;
			j += 1;
		}
		if (retCode != ERROR_NO_MORE_ITEMS)
			printf("regutil:readDataFromRegistry error enumerating values for key %s\n", subKeyName);
#ifdef DEBUG
		printf("regutil:readDataFromRegistry debug key value read: %s\n", keyBuf);
#endif
		if ((pKeyData->next = (struct keyData *)malloc(sizeof(struct keyData))) == NULL) {
			printf("regutil:readDataFromRegistry memory allocation error\n");
			retCode = ERROR_NOT_ENOUGH_MEMORY;
			break;
		}
		pKeyData = pKeyData->next;
		pKeyData->next = NULL;
		pKeyData->keyEnums = NULL;
		pKeyData->keyName = NULL;
		i += 1;
	}
	RegCloseKey(hKey);
	if (retCode == ERROR_NO_MORE_ITEMS)
		retCode = ERROR_SUCCESS;
	
	return retCode;
}