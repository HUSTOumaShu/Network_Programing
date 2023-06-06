#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;
typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

char buffer[1024];

int main()
{
    // Khai bao dia chi server & client
    sockaddr_in saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = 0;    // listen from any port of clients
    saddr.sin_port = htons(8888); // server listen at port 8888
    int clen = sizeof(saddr);

    // Khai bao socket
    int tcp_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // TCP - socket le tan
    int udp_s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // UDP - socket

    /*
    Program running in server
    */
    int error = bind(tcp_s, (sockaddr *)&saddr, sizeof(sockaddr)); // bind socket
    if (error == 0)
    {
        listen(tcp_s, 10);                                                 // the maximum number of clients is 10
        int tcp_c = accept(tcp_s, (sockaddr *)&caddr, (socklen_t *)&clen); // accept client with ip address caddr

        // Send message from server to client
        const char *msg = "Hello World!";
        int sent = send(tcp_c, msg, strlen(msg), 0);
        printf("Sent %d bytes to %s: %s\n", sent, inet_ntoa(caddr.sin_addr), msg);

        // Received message from client and save to buffer
        memset(buffer, 0, sizeof(buffer));
        int received = recv(tcp_c, buffer, sizeof(buffer) - 1, 0);
        printf("Received %d bytes from %s: %s\n", received, inet_ntoa(caddr.sin_addr), buffer);
        close(tcp_c);
    }
    else
        cout << "Binding error! Please check your IP and port!";
    close(tcp_s);

    /*
    Program running in client
    */
    connect(tcp_s, (sockaddr *)&saddr, sizeof(sockaddr));
    const char *message = "From TCP client";
    int sent = send(tcp_s, message, strlen(message), 0);
    printf("Sent %d bytes\n", sent);

    memset(buffer, 0, sizeof(buffer));
    int received = recv(tcp_s, buffer, sizeof(buffer) - 1, 0);
    printf("Received %d bytes: %s", received, inet_ntoa(saddr.sin_addr));

    /*
    File
    */

    // Get content of the file and save to content
    FILE *data = fopen("data.dat", "rb");
    fseek(data, 0, SEEK_END); // put the read point (data) to the end of content's file
    int length = ftell(data); // get the length of the content's file
    char *content = (char *)calloc(length, 1);
    fseek(data, 0, SEEK_SET);        // put the read point (data) to the head of content's file
    fread(content, 1, length, data); // copy content's data to point content
    fclose(data);                    // close the file

    // Reset the content of the file
    FILE *data = fopen("data.dat", "wb");
    fclose(data);

    // Edit content of the file
    char *m = "Message needed to add to the content's file\n";
    FILE *data = fopen("data.dat", "wb");
    fwrite(m, 1, strlen(m), data);
    fclose(data);

// select() function
#include <sys/select.h>
#include <sys/time.h>
    int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout);
    // - maxfdp1 - maximum number of sockets
    // - mang tham do 3 su kien chinh
    // + readset: du lieu di den may tham do
    // + writeset: du lieu di ra khoi may tham do
    // + exceptset: du lieu xu ly ket noi ngat
    // - timeout: thoi gian treo den khi ket thuc select

    // mot so ham su dung voi fd_set

    // void FD_ZERO(fd_set* fdset); // xoa het socket trong fdset
    // void FD_SET(int fd, fd_set* fdset); // them 1 phan tu socket vao trong fdset
    // void FD_CLR(int fd, fd_set* fdset); // xoa phan tu socket fd khoi fdset
    // void FD_ISSET(int fd, fd_set* fdset); // kiem tra socket fd co xay ra su kien trong fdset
}
