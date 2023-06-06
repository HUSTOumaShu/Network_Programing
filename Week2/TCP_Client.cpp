#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

const char *ip = "111.65.250.2"; // ip address of vnexpress.net
typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

int main()
{
    // Initialize socket
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Address
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(s, (sockaddr *)&addr, sizeof(sockaddr)); // connect

    // Send message
    const char *message = "GET / HTTP/1.1\r\nHost: vnexpress.net\r\n\r\n";
    int sent = send(s, message, sizeof(message) - 1, 0); // sent
    printf("Sent %d bytes\n", sent);

    // Receive message
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int received = recv(s, buffer, sizeof(buffer) - 1, 0); // received
    printf("Recieved %d bytes: %s\n", received, buffer);

    close(s);
    memset(buffer, 0, sizeof(buffer) - 1);
}
