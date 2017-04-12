#include <Windows.h>

#define DEFAULT_BUFLEN 512
#define USERAGENT "HTMLGET 1.0"

char *build_get_query(char *host, char *page, char * port);
unsigned int parseArgs(int argc, char * const argv[], char ** host, char ** port, char ** url);
void usage(char* lpszProgramName);
