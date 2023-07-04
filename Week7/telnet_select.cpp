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

vector<int> clients;
vector<int> status;

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
            // Restore data
            FD_ZERO(&read_set);
            FD_SET(s, &read_set);
            for (unsigned int i = 0; i < clients.size(); i++)
                if (status[i] == 1)
                    FD_SET(clients[i], &read_set);

            // Khoi tao select
            select(FD_SETSIZE, &read_set, NULL, NULL, NULL);

            // Check event on data socket
            for (unsigned int i = 0; i < clients.size(); i++)
            {
                if (FD_ISSET(clients[i], &read_set) && status[i] == 1)
                {
                    char buffer[1024] = {0};
                    int received = recv(clients[i], buffer, sizeof(buffer) - 1, 0);
                    if (received > 0 && status[i] == 1)
                    {
                        printf("Received %d bytes from client %d\n", received, clients[i]);
                        if (buffer[strlen(buffer) - 1] == '\n')
                            buffer[strlen(buffer) - 1] = '\0';
                        sprintf(buffer + strlen(buffer), " > command.txt");
                        system(buffer);

                        FILE *f = fopen("command.txt", "rb");
                        fseek(f, 0, SEEK_END);
                        int size = ftell(f);
                        char *data = (char *)calloc(size, 1);
                        fseek(f, 0, SEEK_SET);
                        fread(data, 1, size, f);
                        fclose(f);

                        int sent = 0;
                        int tmp = 0;
                        do
                        {
                            tmp = send(clients[i], data, size, 0);
                            sent += tmp;
                        } while (tmp > 0 && sent < size);

                        printf("Sent %d byte to client %d\n", sent, clients[i]);
                        free(data);
                        data = NULL;
                    }
                    else
                    {
                        printf("Client %d out!\n", clients[i]);
                        status[i] = 0;
                    }
                }
            }

            // Check event on socket s
            if (FD_ISSET(s, &read_set))
            {
                int c = accept(s, (sockaddr *)&caddr, (socklen_t *)&clen);
                clients.push_back(c);
                status.push_back(1);
                printf("Socket %d connect.\n", c);
                const char *msg = "Type your command: ";
                send(c, msg, strlen(msg), 0);
            }
        }
    }
    else
        printf("Bind error! Check you port!\n");
}
