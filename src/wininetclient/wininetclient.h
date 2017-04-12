#pragma comment( lib, "wininet" )

#define DEFAULT_BUFLEN 512

unsigned int parseArgs(int argc, char * const argv[], char ** host, char ** port, char ** url);
void usage(char* lpszProgramName);