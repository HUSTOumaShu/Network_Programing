#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

int main()
{
    // Server's address & Client's address
    sockaddr_in saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = 0;

    // Initialize socket s
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int error = bind(s, (sockaddr *)&saddr, sizeof(sockaddr)); // bind
    if (error == 0)
    {
        listen(s, 10); // listen

        // create data socket
        int c = accept(s, (sockaddr *)&caddr, (socklen_t *)&clen);

        const char *message = "Hello World!\n";
        int sent = send(c, message, strlen(message), 0); // sent
        printf("Sent %d bytes: \n", sent);

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer) - 1);
        int received = recv(c, buffer, sizeof(buffer) - 1, 0); // received
        printf("Received %d bytes: %s", received, buffer);

        close(c);
    }
    else
        printf("Bind error!");
    close(s);
}
