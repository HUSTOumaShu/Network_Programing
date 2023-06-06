#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <malloc.h>
#include <string.h>
using namespace std;

char *domain = "vnexpress.net";
struct addrinfo *res = NULL;

int main()
{

    int error = getaddrinfo(domain, NULL, NULL, &res);
    if (error == 0)
    {
        while (res != NULL)
        {
            cout << "...";
            sockaddr_in *sin = (sockaddr_in *)res->ai_addr;
            res = res->ai_next;
            if (sin->sin_family == AF_INET)
            {
                char *ip = (char *)&(sin->sin_addr.s_addr);
                cout << ip;
            }
            else if (sin->sin_family == AF_INET6)
            {
                cout << "IPv6";
            }
        }
    }
    else
    {
        cout << "Error!";
    }
}