#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>

using namespace std;
typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

vector<int> client_list;
pthread_mutex_t *mutex = NULL;

/*
Using multi-thread to connect more than 1 client serving Telnet
*/
void *processClient(void *arg)
{
    int c = *((int *)arg);
    free(arg);
    arg = NULL;
    while (true)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int received = recv(c, buffer, sizeof(buffer) - 1, 0);
        if (received > 0)
        {
            printf("Received %d byte from client %d: %s", received, c, buffer);
            if (buffer[strlen(buffer) - 1] == '\n')
            {
                buffer[strlen(buffer) - 1] = '\0';
                sprintf(buffer + strlen(buffer), " > command.txt");

                // Using mutex to prevent file conflict
                pthread_mutex_lock(mutex);
                system(buffer);
                pthread_mutex_unlock(mutex);
                /*****************/

                FILE *f = fopen("command.txt", "rb");
                fseek(f, 0, SEEK_END);
                int len = ftell(f);
                char *data = (char *)calloc(len, 1);
                fseek(f, 0, SEEK_SET);
                fread(data, 1, len, f);
                fclose(f);

                int sent = 0;
                int tmp = 0;
                do
                {
                    tmp = send(c, data, len, 0);
                    sent += tmp;
                } while (sent < len && tmp > 0);
                printf("Send %d byte!\n", sent);
                free(data);
                data = NULL;
            }
        }
        else
        {
            break;
        }
    }
    printf("Client %d out!\n", c);
    return NULL;
}

int main()
{
    mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);

    sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6000);
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
            const char *msg = "Type your command: ";
            int sent = send(c, msg, strlen(msg), 0);

            // Multi-thread processing
            pthread_t tid;
            int *arg = (int *)calloc(1, sizeof(int));
            *arg = c;
            pthread_create(&tid, NULL, processClient, arg);
        }
    }
    else
    {
        cout << "Bind error!\n";
    }
    pthread_mutex_destroy(mutex);
    close(s);
}
