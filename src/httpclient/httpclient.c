/**
 * \file httpclient.c
 * \author Laura Le Padellec
 * \date 12 April 2017
 * \brief implemenentation of CNAM SEC102 httpclient assignment
 *
 * Usage: httpclient <host> <port> <url>
 * 
 * host: the HTTP host, for example www.cnam.fr
 * port: the HTTP port or HTTPS port number, for example 80
 * url: the URL to receive via the GET command, for example index.html
 *
 * The program will send a simple HTTP GET request to the host indicated with the
 * URL indicated. The content of the page retrieved will be sent to stdout so if
 * you want to write the contents to a file you should redirect stdout to a file:
 * > httpclient www.mywebsite.com 80 index.html > output.html
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")


#include "httpclient.h"

 /**
 * \brief Build the HTTP GET command.
 * \param _In_ host: host to send the request to
 * \param _Out_ page: string indicating the url to get
 * \param _In_ port: port to connect to - only 80 is supported
 * \return the HTTP GET query or NULL
 */
char *build_get_query(char *host, char *page, char * port) {
	char *query;
	char *getpage = page;
	char *tpl = NULL;

	if (!strcmp(port, "80"))
		tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
	else {
		printf("This program only works for HTTP requests!\n");
		return NULL;
	}
	if (getpage[0] == '/') {
		getpage = getpage + 1;
		fprintf(stderr, "Removing leading \"/\", converting %s to %s\n", page, getpage);
	}
	// -5 is to consider the %s %s %s in tpl and the ending \0
	query = (char *)malloc(strlen(host) + strlen(getpage) + strlen(USERAGENT) + strlen(tpl) - 5);
	sprintf(query, tpl, getpage, host, USERAGENT);
	return query;
}

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
		fprintf(stderr,"Error missing parameters!\n");
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

	fprintf(stderr, "Usage : %s <host> <port> <url>\n", lpszProgramName);
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
	unsigned int			retCode = ERROR_GEN_FAILURE;
	char *					host = NULL;
	char * 					port = NULL;
	char *					url = NULL;
	WSADATA					wsaData;
	struct addrinfo			*result = NULL, *ptr = NULL, hints;
	SOCKET					ConnectSocket = INVALID_SOCKET;
	char					recvbuf[DEFAULT_BUFLEN];
	char *					sendData;
	int						recvbuflen = DEFAULT_BUFLEN;

	retCode = parseArgs(argc, argv, &host, &port, &url);
	if (retCode != ERROR_SUCCESS)
		return retCode;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		fprintf(stderr, "WSAStartup failed: %d\n", iResult);
		return 1;
	}
	// set up the addrinfo structure, a stream socket will be created using the
	// TCP/IP protocol, the IP family can be IPV4 or IPV6

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the host address and port
	iResult = getaddrinfo(host, port, &hints, &result);
	if (iResult != 0) {
		fprintf(stderr, "getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	//check for errors in socket initialization
	if (ConnectSocket == INVALID_SOCKET) {
		retCode = WSAGetLastError();
		fprintf(stderr, "Error at socket(): %ld\n", retCode);
		freeaddrinfo(result);
		WSACleanup();
		return retCode;
	}

	// Connect to server.
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		fprintf(stderr, "Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	if ((sendData = build_get_query(host, url, port)) == NULL) {
		fprintf(stderr, "Unable to process requests on ports other than HTTP port 80\n");
		return 1;
	}
	// Send an initial buffer
	iResult = send(ConnectSocket, sendData, (int)strlen(sendData), 0);
	if (iResult == SOCKET_ERROR) {
		fprintf(stderr, "send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		fprintf(stderr, "shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive data until the server closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			fprintf(stdout, recvbuf);
		else if (iResult == 0)
			fprintf(stderr, "Connection closed\n");
		else
			fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
	} while (iResult > 0);

	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		fprintf(stderr, "shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return retCode;
}	

