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
        // Process 1 - Recvfrom clients broadcast IP & name

        sockaddr_in saddr, caddr;
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(6000);
        saddr.sin_addr.s_addr = 0;
        int clen = sizeof(caddr);

        int s1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        int error = bind(s1, (sockaddr *)&saddr, sizeof(sockaddr));
        char buffer[1024];
        if (error == 0)
        {
            while (true)
            {
                memset(buffer, 0, sizeof(buffer));
                int received = recvfrom(s1, buffer, sizeof(buffer) - 1, 0, (sockaddr *)&caddr, (socklen_t *)&clen);
                if (buffer[strlen(buffer) - 1] == '\n')
                {
                    buffer[strlen(buffer) - 1] = '\0';
                }
                FILE *f = fopen("clients.txt", "at");
                fprintf(f, "%s %s\n", buffer, inet_ntoa(caddr.sin_addr));
                fclose(f);

                // ACK from server
                sendto(s1, "ACK", 3, 0, (sockaddr *)&caddr, sizeof(caddr));
                exit(0);
            }
        }
        else
            cout << "Bind error!\n";
    }
    else
    {
        if (fork() == 0)
        {
            // Process 2
            sockaddr_in saddr, caddr;
            int clen = sizeof(caddr);
            saddr.sin_family = AF_INET;
            saddr.sin_port = htons(5000);
            saddr.sin_addr.s_addr = 0;

            // Initialize socket s
            int s2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            int error = bind(s2, (sockaddr *)&saddr, sizeof(sockaddr)); // bind
            if (error == 0)
            {
                listen(s2, 10);
                while (true)
                {
                    int c = accept(s2, (sockaddr *)&caddr, (socklen_t *)&clen);
                    FILE *f = fopen("clients.txt", "rb");
                    fseek(f, 0, SEEK_END);
                    int length = ftell(f);
                    fseek(f, 0, SEEK_SET);
                    char *data = (char *)calloc(length + 1, 1);
                    fread(data, 1, length, f);
                    fclose(f);
                    int sent = 0;
                    while (sent < length)
                    {
                        int tmp = send(c, data + sent, length - sent, 0);
                        if (tmp > 0)
                        {
                            sent += tmp;
                        }
                        else
                            break;
                    }
                    close(c);
                    free(data);
                    data = NULL;
                }
            }
            exit(0);
        }
    }
    cout << "Press any key to exit!" << endl;
    getchar();
}
