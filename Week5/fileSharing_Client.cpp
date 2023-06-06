#include <iostream>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#include <signal.h>
#include <stdlib.h>

#include <sys/wait.h>

using namespace std;

typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

int main()
{
    if (fork() == 0)
    {
        // Process 1 - each 5 second, client (broadcast) send to server to express its status
        char name[1024] = {0};
        printf("Client name: ");
        fgets(name, sizeof(name) - 1, stdin);
        int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        int broadcast_on = 1;
        setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast_on, strlen(name));

        sockaddr_in baddr, myaddr;
        // client received ACK from server at port 6000
        myaddr.sin_family = AF_INET;
        myaddr.sin_port = htons(6000);
        myaddr.sin_addr.s_addr = 0;
        bind(s, (sockaddr *)&myaddr, sizeof(myaddr));

        baddr.sin_family = AF_INET;
        baddr.sin_port = htons(6000);
        baddr.sin_addr.s_addr = inet_addr("255.255.255.255");

        while (true)
        {
            sendto(s, name, strlen(name), 0, (sockaddr *)&baddr, sizeof(baddr));
            sleep(5);
        }
        exit(0);
    }
    else
    {
        if (fork() == 0)
        {
            // Process 2
            sockaddr_in myaddr, server;
            myaddr.sin_family = AF_INET;
            myaddr.sin_port = htons(6000);
            myaddr.sin_addr.s_addr = 0;
            int slen = sizeof(server);

            // create udp packet to received ACK packet of server at port 6000
            int udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            bind(udp, (sockaddr *)&myaddr, sizeof(myaddr));

            // get IP of server from ACK packet server sent
            char ack[1024] = {0};
            recvfrom(udp, ack, sizeof(ack) - 1, 0, (sockaddr *)&server, (socklen_t *)&slen);

            server.sin_port = htons(5000); // change to TCP port 5000
            int c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            connect(c, (sockaddr *)&server, sizeof(server));
            char data[1024];
            while (true)
            {
                memset(data, 0, sizeof(data));
                int tmp = recv(c, data, sizeof(data) - 1, 0);
                if (tmp <= 0)
                    break;
                else
                    printf("%s", data);
            }

            exit(0);
        }
    }
}
