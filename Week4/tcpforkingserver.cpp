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

void handler(int signum)
{
    if (signum == SIGCHLD)
    {
        int status = 0;
        pid_t pid;
        // wait
        while ((pid = wait(&status)) > 0)
        {
            printf("Child process %d has terminated\n");
        }
        printf("Exit 1 loop!\n");
        // waitpid
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        {
            printf("Child process %d has terminated\n");
        }
        printf("Exit 1 loop!\n");
    }
}

void sighandler(int signum)
{
    if (signum == SIGTERM)
    {
        // Get content of message group from dataf
        FILE *dataf = fopen("data.dat", "rb");
        fseek(dataf, 0, SEEK_END);
        int length = ftell(dataf);
        char *data = (char *)calloc(length + 1, 1);
        fseek(dataf, 0, SEEK_SET);
        fread(data, 1, length, dataf);
        fclose(dataf);

        // Send to clients the content
        FILE *clients = fopen("clients.dat", "rb");
        while (!feof(clients))
        {
            int tmp = 0;
            fread(&tmp, sizeof(int), 1, clients);
            send(tmp, data, strlen(data), 0);
            printf("Sent to %d\n", tmp);
        }
        fclose(clients);
        free(data);
        data = NULL;
    }
}

int main()
{
    int parentID = getpid();
    signal(SIGTERM, sighandler);

    FILE *clients = fopen("clients.dat", "wb");
    fclose(clients);

    sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = 0;
    saddr.sin_port = htons(5000);
    int clen = sizeof(caddr);

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int error = bind(s, (sockaddr *)&saddr, sizeof(sockaddr)); // bind
    if (error == 0)
    {
        listen(s, 10);
        while (true)
        {
            int c = accept(s, (sockaddr *)&caddr, (socklen_t *)&clen);
            // Add client accepted to clients file
            FILE *clients = fopen("clients.dat", "ab");
            fwrite(&c, sizeof(int), 1, clients);
            fclose(clients);

            if (fork() == 0)
            {
                printf("Receiving data...\n");
                while (true)
                {
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    int received = recv(c, buffer, sizeof(buffer) - 1, 0);
                    if (received > 0)
                    {
                        printf("Received %d bytes from %d: %s\n", received, c, buffer);
                        // Update data of message group and send to clients
                        FILE *f = fopen("data.dat", "wb");
                        fwrite(buffer, 1, strlen(buffer), f);
                        fclose(f);
                        kill(parentID, SIGTERM);
                        exit(0);
                    }
                    else
                        break;
                }
            }
        }
    }
    else
        printf("Bind error!");
    close(s);
}
