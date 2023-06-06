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

int main(int argc, char **argv)
{
    /*
    Input terminal included 3 parameters (name of program, ip address & port of server)
    */
    if (argc >= 3)
    {
        in_addr_t ip = inet_addr(argv[1]);
        short port = (short)atoi(argv[2]);

        int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        sockaddr_in saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr.s_addr = ip;

        int error = connect(s, (sockaddr *)&saddr, sizeof(sockaddr));

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int received = 0;
        int tmp = 0;
        int bytes_to_received = 0;
        do
        {
            bytes_to_received = sizeof(buffer) - received - 1;
            tmp = recv(s, buffer + received, bytes_to_received, 0);
            received += tmp;
        } while ((tmp >= 0) && (tmp == bytes_to_received));
        printf("%s\n", buffer);

        while (error == 0)
        {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer) - 1, stdin);
            send(s, buffer, strlen(buffer), 0);

            received = 0;
            char *response = NULL;
            do
            {
                memset(buffer, 0, sizeof(buffer));
                tmp = recv(s, buffer, sizeof(buffer) - 1, 0);

                response = (char *)realloc(response, received + tmp + 1);
                sprintf(response + strlen(response), "%s", buffer);
                response[received + tmp] = '\0';
                received += tmp;
            } while ((tmp >= 0) && (tmp == sizeof(buffer) - 1));
            printf("Received %d bytes:\n%s\n", bytes_to_received, response);
            free(response);
            response = NULL;
        }
    }
    else
        printf("Parametter missing!\n");
}
