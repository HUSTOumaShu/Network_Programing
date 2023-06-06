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

void *processThread(void *arg)
{
    int c = *((int *)arg);
    client_list.push_back(c);
    free(arg);
    arg = NULL;
    while (true)
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int received = recv(c, buffer, sizeof(buffer) - 1, 0);
        if (received > 0) // tranh truong hop lap vo tan vong lap while
        {
            printf("Received from %d: %s\n", c, buffer);
            for (int i = 0; i < client_list.size(); i++)
            {
                if (client_list[i] != c)
                    send(client_list[i], buffer, strlen(buffer), 0);
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
    sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
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

            // Xu ly da luong
            pthread_t tid;
            int *arg = (int *)calloc(1, sizeof(int));
            *arg = c;
            pthread_create(&tid, NULL, processThread, arg);
        }
    }
    else
        cout << "Bind error! Check port!!\n";
}
