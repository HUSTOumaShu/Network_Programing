#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
char currentPath[1024] = {0};
int CompareDR(const struct dirent **A, const struct dirent **B)
{
    if ((*A)->d_type == DT_DIR)
    {
        if ((*B)->d_type == DT_DIR)
        {
            return strcasecmp((*B)->d_name, (*A)->d_name);
        }
        else
            return 1;
    }
    else
    {
        if ((*B)->d_type != DT_DIR)
        {
            return strcasecmp((*B)->d_name, (*A)->d_name);
        }
        else
            return 0;
    }
}
void Append(char **phtml, const char *name)
{
    int oldLen = *phtml == NULL ? 0 : strlen(*phtml);              // Tính số ký tự cữ
    (*phtml) = (char *)realloc(*phtml, oldLen + strlen(name) + 1); // Cấp phát thêm
    memset(*phtml + oldLen, 0, strlen(name) + 1);                  // Điền 0 vào các byte nhớ đc cấp phát thêm
    sprintf(*phtml + oldLen, "%s", name);                          // Nối xâu
}

char *BuildHTML(char *path)
{
    char *html = NULL;
    struct dirent **namelist;
    int n;

    n = scandir(path, &namelist, NULL, CompareDR);
    Append(&html, "<html>\n");
    while (n--)
    {
        if (namelist[n]->d_type == DT_DIR)
        {
            Append(&html, "<a href = \"");
            Append(&html, namelist[n]->d_name);
            Append(&html, "*");
            Append(&html, "\"><b>");
            Append(&html, namelist[n]->d_name);
            Append(&html, "</b></a><br>\n");
        }
        else
        {
            Append(&html, "<a href = \"");
            Append(&html, namelist[n]->d_name);
            Append(&html, "\"><i>");
            Append(&html, namelist[n]->d_name);
            Append(&html, "</i></a><br>\n");
        }
        free(namelist[n]);
    }
    Append(&html, "</html>\n");
    free(namelist);
    namelist = NULL;
    return html;
}

void Send(int c, char *data, int len)
{
    int sent = 0;
    while (sent < len)
    {
        int tmp = send(c, data + sent, len - sent, 0);
        if (tmp > 0)
        {
            sent += tmp;
        }
        else
            break;
    }
}

void *ClientThread(void *arg)
{
    int *tmp = (int *)arg;
    int c = *tmp;
    free(tmp);
    tmp = NULL;
    char *buffer = NULL;
    char tmpc;
    while (0 == 0)
    {
        int r = recv(c, &tmpc, 1, 0);
        if (r > 0)
        {
            if (tmpc != '\r' && tmpc != '\n')
            {
                int oldLen = buffer == NULL ? 0 : strlen(buffer);
                buffer = (char *)realloc(buffer, oldLen + 2);
                buffer[oldLen] = tmpc;
                buffer[oldLen + 1] = 0; // Linh canh
            }
            else
                break;
        }
        else
            break;
    }
    if (strlen(buffer) > 0)
    {
        printf("Socket %d, Received: %s\n", c, buffer);
        char method[1024] = {0};
        char path[1024] = {0};
        char ver[1024] = {0};
        sscanf(buffer, "%s%s%s", method, path, ver);
        if (strcmp(method, "GET") == 0)
        {
            while (strstr(path, "%20") != NULL)
            {
                char *tmp = strstr(path, "%20");
                tmp[0] = ' ';
                strcpy(tmp + 1, tmp + 3);
            }
            if (strcmp(path, "/") == 0)
            {
                char *html = BuildHTML(currentPath);
                char httpok[1024] = {0};
                sprintf(httpok, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\n\r\n", (int)strlen(html));
                Send(c, (char *)httpok, strlen(httpok));
                Send(c, (char *)html, strlen(html));
            }
            else
            {
                if (strstr(path, "*") != NULL) // FOLDER
                {
                    if (currentPath[strlen(currentPath) - 1] == '/')
                    {
                        currentPath[strlen(currentPath) - 1] = 0;
                    }

                    path[strlen(path) - 1] = 0; // Bo * o cuoi di
                    if (strcmp(path, "/.") == 0)
                    {
                        // DO NOTHING
                    }
                    else if (strcmp(path, "/..") == 0)
                    {
                        int i = strlen(currentPath) - 1;
                        while (i >= 0)
                        {
                            if (currentPath[i] != '/')
                            {
                                currentPath[i] = 0;
                            }
                            else
                                break;
                            i -= 1;
                        }
                    }
                    else
                    {
                        sprintf(currentPath + strlen(currentPath), "%s", path);
                    }

                    if (strlen(currentPath) == 0)
                    {
                        strcpy(currentPath, "/");
                    }
                    char *html = BuildHTML(currentPath);
                    char httpok[1024] = {0};
                    sprintf(httpok, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", (int)strlen(html));
                    Send(c, (char *)httpok, strlen(httpok));
                    Send(c, (char *)html, strlen(html));
                }
                else // FILE
                {
                    if (currentPath[strlen(currentPath) - 1] == '/')
                    {
                        currentPath[strlen(currentPath) - 1] = 0;
                    }
                    sprintf(currentPath + strlen(currentPath), "%s", path);

                    FILE *f = fopen(currentPath, "rb");
                    if (f != NULL)
                    {
                        fseek(f, 0, SEEK_END);
                        int size = ftell(f);
                        fseek(f, 0, SEEK_SET);
                        char *data = (char *)calloc(size, 1);
                        fread(data, 1, size, f);
                        char httpok[1024] = {0};
                        sprintf(httpok, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", size);
                        Send(c, httpok, strlen(httpok));
                        Send(c, data, size);
                    }
                    else
                    {
                        // SEND 404 NOT FOUND
                    }
                    fclose(f);
                }
            }
        }
        else if (strcmp(method, "POST") == 0)
        {
        }
    }
    return NULL;
}
int main()
{
    strcpy(currentPath, "/home/huytr");
    SOCKADDR_IN myaddr, caddr;
    int clen = sizeof(caddr);
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(5000);
    myaddr.sin_addr.s_addr = 0;
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(s, (SOCKADDR *)&myaddr, sizeof(myaddr));
    listen(s, 10);
    while (0 == 0)
    {
        int c = accept(s, (SOCKADDR *)&caddr, (socklen_t *)&clen);
        pthread_t tid;
        int *arg = (int *)calloc(1, sizeof(int));
        *arg = c;
        pthread_create(&tid, NULL, ClientThread, (void *)arg);
    }
}
