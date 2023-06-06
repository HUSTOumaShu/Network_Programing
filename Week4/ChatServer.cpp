#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>
using namespace std;

typedef sockaddr_in sockaddr_in;
typedef sockaddr sockaddr;

char msg[1024];

/* Ctrl + C to stop process*/
// void sighandler(int signum)
// {
//     if (signum == SIGINT)
//     {
//         cout << "Terminating...\n";
//         exit(0);
//     }
// }

void sighandler(int signum)
{
    // sigterm (signal terminal - shut down the process )
    if (signum == SIGTERM)
    {
        // get the content of file data and save to content
        FILE *data = fopen("data.dat", "rb");
        fseek(data, 0, SEEK_END);
        int length = ftell(data);
        char *content = (char *)calloc(length, 1);
        fseek(data, length, 0);
        fread(data, 1, length, stdin);
        fclose(data);

        // get the clients and send content to each client
        FILE *clients = fopen("clients", "rb");
        while (!feof(clients))
        {
            int tmp = 0; // socket temp
            fread(&tmp, sizeof(int), 1, clients);
            printf("Send to %d\n", tmp);
            send(tmp, content, sizeof(content), 0);
        }
        fclose(clients);
        free(content);
        content = NULL;
    }
}

int main()
{
    int parentID = getpid();
    signal(SIGTERM, sighandler);

    FILE *clients = fopen("clients.dat", "wb"); // Reset the clients.dat
    fclose(clients);

    sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = 0;
    int clen = sizeof(sockaddr);

    // Process socket
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(s, (sockaddr *)&saddr, sizeof(sockaddr));
    listen(s, 10);
    while (true)
    {
        int c = accept(s, (sockaddr *)&caddr, (socklen_t *)&clen);
        FILE *clients = fopen("clients.dat", "ab");
        fwrite(&c, sizeof(int), 1, clients);
        fclose(clients);

        if (fork() == 0)
        {
            close(s); // close the socket s in the child process
            printf("Receiving data...\n");
            while (true)
            {
                memset(msg, 0, sizeof(msg));
                int rec = recv(c, msg, strlen(msg), 0);
                if (rec > 0)
                {
                    printf("Received %d bytes from %s: %s", rec, inet_ntoa(caddr.sin_addr), msg);
                    FILE *data = fopen("data.dat", "wb");
                    fwrite(msg, 1, strlen(msg), data);
                    fclose(data);
                    kill(parentID, SIGTERM);
                }
            }
        }

        // exit(0);
    }
}
