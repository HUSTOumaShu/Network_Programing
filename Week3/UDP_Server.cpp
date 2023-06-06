#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
using namespace std;

typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

char msg[1024];
vector<sockaddr_in> senders;

bool check(const vector<sockaddr_in> senders, sockaddr_in sender)
{
    bool res = true;
    if (!senders.empty())
    {
        for (unsigned int i = 0; i < senders.size(); i++)
        {
            if (senders[i].sin_addr.s_addr == sender.sin_addr.s_addr)
            {
                res = false;
                break;
            }
        }
    }
    return res;
}

int main()
{
    sockaddr_in raddr;
    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(5000);
    raddr.sin_addr.s_addr = 0;

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bind(s, (sockaddr *)&raddr, sizeof(sockaddr));
    while (true)
    {
        memset(msg, 0, sizeof(msg));
        sockaddr_in sender;
        int clen = sizeof(sockaddr);
        int rec = recvfrom(s, msg, sizeof(msg) - 1, 0, (sockaddr *)&sender, (socklen_t *)&clen);
        printf("Received %d bytes from address %s: %s", rec, inet_ntoa(sender.sin_addr), msg);
        if (check(senders, sender))
            senders.push_back(sender);

        for (auto &x : senders)
        {
            sendto(s, msg, strlen(msg), 0, (sockaddr *)&(x), sizeof(sockaddr));
            printf("Sent to address %s", inet_ntoa(sender.sin_addr));
        }
    }
}
