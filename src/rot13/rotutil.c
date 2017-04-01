#include <Windows.h>
#include <stdio.h>

#include "rotutil.h"

unsigned int encodeRot13(char * strToEncode, char * result) {
	int		key, length;
	LPTSTR	locStr = strToEncode;
	/* we will begin by capitalizing the string */
	CharUpper(locStr);
	length = strlen(locStr);
	
	key = 13;
	/* 
	 * loop through the string and add 13 to/from each character - if the value
	 * exceeds 26 then perform a modulo 2 to calculate the correct character
	 */ 
	for (int i = 0; i < length; i++)
	{
		int currentLetter = locStr[i];
		char cipher = currentLetter + key;

		if ((currentLetter - 'A') + key >= 26)
		{
			key = ((currentLetter - 'A') + key) % 26;
			cipher = 'A' + key;
		}

		locStr[i] = (char)cipher;
		/* reset the key and do the next letter */
		key = 13;
	}
	/* copy the results to the return string and return */
	lstrcpy(result, locStr);
	return strlen(result);
}
