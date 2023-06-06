#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
using namespace std;

typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

int main(int argc, char **argv)
{
    /*
    Input terminal included 2 parameter (name of program & port for client to connect)
    */
    if (argc > 1)
    {
        short port = atoi(argv[1]);

        // Server's address & Client's address
        sockaddr_in saddr, caddr;
        int clen = sizeof(saddr);

        // Initialize socket
        int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        // Address of server
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = 0;

        // Bind socket
        int error = bind(s, (sockaddr *)&saddr, sizeof(sockaddr));
        if (error == 0)
        {
            listen(s, 10);

            int c = accept(s, (sockaddr *)&caddr, (socklen_t *)&clen);
            const char *message = "Type your command: ";
            int sent = send(c, message, strlen(message), 0);
            printf("Send %d bytes!\n", sent);

            while (true)
            {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer) - 1);
                int received = recv(c, buffer, sizeof(buffer) - 1, 0);
                if (received > 0)
                {
                    printf("Received %d bytes: %s", received, buffer);
                    if (buffer[strlen(buffer) - 1] == '\n')
                    {
                        buffer[strlen(buffer) - 1] = '\0';
                        sprintf(buffer + strlen(buffer), " > command.txt");
                        system(buffer);

                        FILE *f = fopen("command.txt", "rb");

                        // get size of file
                        fseek(f, 0, SEEK_END);
                        int size = ftell(f);

                        // get data of file
                        char *data = (char *)calloc(size, 1);
                        fseek(f, 0, SEEK_SET);
                        fread(data, 1, size, f);
                        fclose(f);

                        // send data
                        int sent = 0;
                        int tmp = 0;
                        do
                        {
                            tmp = send(c, data, size, 0);
                            sent += tmp;
                        } while (sent < size && tmp >= 0);
                        printf("Send: %s", data);
                        free(data);
                        data = NULL;
                    }
                }
            }
            close(c);
        }
        else
            printf("Bind error!\n");
    }
    else
        printf("Parametter missing!\n");
}
