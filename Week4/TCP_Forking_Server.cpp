#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>
using namespace std;

typedef sockaddr_in sockaddr_in;
typedef sockaddr sockaddr;

using namespace std;

int main()
{
    sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = 0;
    int clen = sizeof(caddr);

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int error = bind(s, (sockaddr *)&saddr, sizeof(sockaddr));
    if (error == 0)
    {
        listen(s, 10);
        while (true)
        {
            int c = accept(s, (sockaddr *)&caddr, (socklen_t *)&clen);
            if (fork() == 0)
            {
                printf("Receiving data...\n");
                char buffer[1024];
                while (true)
                {
                    memset(buffer, 0, sizeof(buffer) - 1);
                    int received = recv(c, buffer, sizeof(buffer) - 1, 0); // received
                    if (received > 0)
                        printf("Received %d bytes: %s", received, buffer);
                }
                exit(0);
            }
        }
    }
    else
        cout << "Bind error!\n";
}
