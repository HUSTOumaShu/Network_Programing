#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <malloc.h>
#include <arpa/inet.h>
using namespace std;

const char *domain = "google.com";
struct addrinfo *res = NULL;

int main()
{
    hostent *phost = gethostbyname(domain);
    if (phost != NULL)
    {
        int i = 0;
        while (phost->h_addr_list[i] != NULL)
        {
            printf("%d.%d.%d.%d", (unsigned char)phost->h_addr_list[i][0],
                   (unsigned char)phost->h_addr_list[i][1],
                   (unsigned char)phost->h_addr_list[i][2],
                   (unsigned char)phost->h_addr_list[i][3]);
            i++;
        }
    }
    printf("...");
}
