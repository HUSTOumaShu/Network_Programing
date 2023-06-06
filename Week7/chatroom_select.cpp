#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <vector>

using namespace std;
typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

vector<int> clients; // list of client sockets
vector<int> status;  // status of client sockets

int main()
{
    sockaddr_in myaddr, caddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(5000);
    myaddr.sin_addr.s_addr = 0;
    int clen = sizeof(caddr);

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int error = bind(s, (sockaddr *)&myaddr, sizeof(sockaddr));
    if (error == 0)
    {
        listen(s, 10);
        fd_set read_set;

        while (true)
        {
            // restore data
            FD_ZERO(&read_set);   // read_set = {}
            FD_SET(s, &read_set); // add socket s to read_set
            for (int i = 0; i < clients.size(); i++)
                if (status[i] == 1)
                    FD_SET(clients[i], &read_set);

            /*
            Khoi tao tham do bang select
            */
            select(FD_SETSIZE, &read_set, NULL, NULL, NULL); // FD_SETSIZE = 1024

            // check event on data socket - receive data
            for (int i = 0; i < clients.size(); i++)
            {
                if (FD_ISSET(clients[i], &read_set) && status[i] == 1)
                {
                    char buffer[1024] = {0};
                    int received = recv(clients[i], buffer, sizeof(buffer) - 1, 0);

                    // send to another client
                    if (received > 0 && status[i] == 1)
                    {
                        printf("Received %d bytes from %d: %s", received, clients[i], buffer);
                        for (int k = 0; k < clients.size(); k++)
                            if (k != i)
                                send(clients[k], buffer, strlen(buffer), 0);
                    }
                    else
                    {
                        printf("Client %d out!\n", clients[i]);
                        status[i] = 0; // clients disconnected
                    }
                }
            }

            // check event on socket s (check read)
            if (FD_ISSET(s, &read_set))
            {
                int c = accept(s, (sockaddr *)&caddr, (socklen_t *)&clen);
                clients.push_back(c);
                status.push_back(1);
                printf("Socket %d connected.\n", c);
            }
        }
    }
    else
        cout << "Bind error!\n";
}
