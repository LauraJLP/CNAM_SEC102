/**
* \file httpclient.c
* \author Laura Le Padellec
* \date 12 April 2017
* \brief implemenentation of CNAM SEC102 wininet client assignment
*
* Usage: wininetclient <host> <port> <url>
*
* host: the HTTP host, for example www.cnam.fr
* port: the HTTP port or HTTPS port number, for example 80
* url: the URL to receive via the GET command, for example index.html
*
* The program will send a simple HTTP GET request to the host indicated with the
* URL indicated. The content of the page retrieved will be sent to stdout so if
* you want to write the contents to a file you should redirect stdout to a file:
* > wininetclient www.mywebsite.com 80 index.html > output.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <wininet.h>
#include <shellapi.h>
#include <mmsystem.h>

#include "wininetclient.h"


/**
* \brief Parse the command line arguments.
* \param _In_ argc: Number of parameters passed to the program
* \param _In_ argv: List of all parameters of type char * const
* \param _Out_ host: string indicating the host
* \param _Out_ port: integer indicating the port number
* \param _Out_ url: string indicating the URL to get
* \return ERROR_SUCCESS or error code indicating the reason for the program failure
* \remark
*/
unsigned int parseArgs(int argc, char * const argv[], char ** host, char ** port, char ** url) {
	int				retErr = ERROR_SUCCESS;

	if (argc == 4) {
		*host = argv[1];
		*port = argv[2];
		*url = argv[3];
	}
	else {
		usage(argv[0]);
		return ERROR_INVALID_PARAMETER;
	}
	return retErr;
}

/**
* \brief Print out a program usage message.
* \param _In_ lpszProgramName: Name of the program
* \remark
*/
void usage(char* lpszProgramName) {
	fprintf(stderr, "Usage : %s <host> <port> <url>\n", lpszProgramName);
}

/**
* \brief This is the main entry point.
* \param _In_ argc: Number of parameters passed to the program
* \param _In_ argv: List of all parameters of type char
* \return ERROR_SUCCESS or error code indicating the reason for the program failure
* \remark
*/
int main(int argc, char * const argv[])
{
	char *			host;
	char *			port;
	char *			url;
	HINTERNET		req, con;
	const char		*accept = "*/*";
	HANDLE			ih = 0;
	char			recvbuf[DEFAULT_BUFLEN];
	int				recvbuflen = DEFAULT_BUFLEN;

	unsigned int retCode = parseArgs(argc, argv, &host, &port, &url);
	if (retCode != ERROR_SUCCESS)
		return retCode;

	ih = InternetOpen("MyWininetClient", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (ih == NULL)	{
		fprintf(stderr, "Error calling InternetOpen()\n");
		return 1;
	}
	con = InternetConnect(ih, host, INTERNET_DEFAULT_HTTP_PORT,
		NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

	req = HttpOpenRequest(con, "GET", url, "HTTP/1.1", NULL, NULL, INTERNET_FLAG_NO_UI, 0);
	if (req == NULL) {
		fprintf(stderr, "%s\n", "error HttpOpenRequest");
		return 1;
	}
	if (HttpSendRequest(req, NULL, 0, NULL, 0)) {
		while (InternetReadFile(req, &recvbuf, DEFAULT_BUFLEN, &recvbuflen) == TRUE) {
			if (recvbuflen == 0)
				break;
			printf("%s", recvbuf);
		}
	}
	else {
		fprintf(stderr, "%s\n", "error HttpSendRequest");
	}
	InternetCloseHandle(ih);
	InternetCloseHandle(req);
	return 0;
}
