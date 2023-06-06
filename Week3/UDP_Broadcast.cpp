#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

char msg[1024];

int main()
{
    const int broadcast_on = 1;
    // Initialize address
    sockaddr_in toadrr, radrr;
    toadrr.sin_family = AF_INET;
    toadrr.sin_port = htons(5000);
    toadrr.sin_addr.s_addr = inet_addr("127.0.0.1");

    radrr.sin_family = AF_INET;
    radrr.sin_port = htons(6000);
    radrr.sin_addr.s_addr = 0;

    // Socket
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast_on, sizeof(broadcast_on));
    bind(s, (sockaddr *)&radrr, sizeof(sockaddr));
    while (true)
    {
        cout << "Enter your message: ";
        memset(msg, 0, sizeof(msg));
        fgets(msg, sizeof(msg) - 1, stdin);
        sendto(s, msg, strlen(msg), 0, (sockaddr *)&toadrr, sizeof(sockaddr));

        memset(msg, 0, sizeof(msg) - 1);
        sockaddr_in sender;
        int clen = sizeof(sockaddr);
        int rec = recvfrom(s, msg, sizeof(msg) - 1, 0, (sockaddr *)&sender, (socklen_t *)&clen);
        printf("Received %d bytes from address %s: %s", rec, inet_ntoa(sender.sin_addr), msg);
    }
    close(s);
}
