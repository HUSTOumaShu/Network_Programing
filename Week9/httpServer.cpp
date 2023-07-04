#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <vector>
#include <malloc.h>
#include <dirent.h>
using namespace std;

typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;
// Get the current path of server
char currentPath[1024] = {0};

/*
** Scandir
*/

//
int compareDR(const struct dirent **A, const struct dirent **B)
{
    if ((*A)->d_type == DT_DIR)
    {
        if ((*B)->d_type == DT_DIR)
            return strcmp((*B)->d_name, (*A)->d_name);
        else
            return 1;
    }
    else
    {
        if ((*B)->d_type != DT_DIR)
            return strcmp((*B)->d_name, (*A)->d_name);
        else
            return 0;
    }
}

void Append(char **phtml, const char *name)
{
    int oldLen = *phtml == NULL ? 0 : strlen(*phtml);
    (*phtml) = (char *)realloc(*phtml, oldLen + strlen(name) + 1);
    memset(*phtml + oldLen, 0, strlen(name) + 1);
    sprintf(*phtml + oldLen, "%s", name);
}

char *createHTML(char *path)
{
    char *html = NULL;
    struct dirent **namelist;
    int n = scandir(path, &namelist, NULL, compareDR);

    if (n == -1)
    {
        printf("Error\n");
    }
    else
    {
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
    }
    return html;
}

// Send data function
void Send(int c, char *data, int length)
{
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
}

void *processThread(void *arg)
{
    int c = *((int *)arg);
    free(arg);
    arg = NULL;

    char *buffer = NULL;
    char tmp;
    while (true)
    {
        // Receive each byte tmp and copy to buffer
        int r = recv(c, &tmp, 1, 0);
        if (r > 0)
        {
            if (tmp != '\r' && tmp != '\n')
            {
                int oldLen = (buffer == NULL) ? 0 : strlen(buffer);
                buffer = (char *)realloc(buffer, oldLen + 2);
                buffer[oldLen] = tmp;
                buffer[oldLen + 1] = '\0';
            }
            else
                break;
        }
        else
            break;
    }
    if (buffer != NULL)
    {
        printf("Received %s\n", buffer);
        // Devided buffer to 3 part: method path version
        char method[1024] = {0};
        char path[1024] = {0};
        char ver[1024] = {0};
        sscanf(buffer, "%s%s%s", method, path, ver);
        if (strcmp(method, "GET") == 0)
        {
            // Process with folder with character ' ' with hyperlink is "%20"
            while (strstr(path, "%20") != NULL)
            {
                int length = strlen(path);
                char *tmp = strstr(path, "%20");
                tmp[0] = ' ';
                strcpy(tmp + 1, tmp + 3); // shift character
            }
            // Root
            if (strcmp(path, "/") == 0)
            {
                char *html = createHTML(currentPath);
                char httpOK[1024] = {0};
                sprintf(httpOK, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\n\r\n", (int)strlen(html));
                Send(c, (char *)httpOK, strlen(httpOK));
                Send(c, (char *)html, strlen(html));
            }

            // Link != root
            // check file or folder by '*' at the end of hyperlink in Append function
            else
            {
                // Process with folder
                if (strstr(path, "*") != NULL) // strstr to find "*" in path
                {
                    path[strlen(path) - 1] = '\0';

                    if (strcmp(path, "/.") == 0) // return the previous folder
                    {
                        // DO NOTHING
                    }

                    else if (strcmp(path, "/..") == 0) // return the root folder
                    {
                        int i = strlen(currentPath) - 1;
                        while (i >= 0)
                        {
                            if (currentPath[i] != '/')
                                currentPath[i] = '\0';
                            else
                                break;
                            i--;
                        }
                        if (strlen(currentPath) == 0)
                            strcmp(currentPath, "/");
                    }
                    else
                    {
                        if (currentPath[strlen(currentPath)] == '/')
                            currentPath[strlen(currentPath)] = '\0';
                        sprintf(currentPath + strlen(currentPath), "%s", path); // add path to the end of current path
                    }
                    char *html = createHTML(currentPath);
                    char httpOK[1024] = {0};
                    sprintf(httpOK, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: Keep-Alive\r\n\r\n", (int)strlen(html));
                    Send(c, (char *)httpOK, strlen(httpOK));
                    Send(c, (char *)html, strlen(html));
                }

                // Process with file
                else // file
                {
                    if (currentPath[strlen(currentPath) - 1] == '/')
                        currentPath[strlen(currentPath) - 1] = '\0';
                    sprintf(currentPath + strlen(currentPath), "%s", path);
                    FILE *f = fopen(currentPath, "rb");
                    if (f != NULL)
                    {
                        fseek(f, 0, SEEK_END);
                        int size = ftell(f);
                        fseek(f, 0, SEEK_SET);
                        char *data = (char *)calloc(size, sizeof(char));
                        fread(data, sizeof(char), size, f);
                        char httpOK[1024] = {0};
                        sprintf(httpOK, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", size);
                        Send(c, httpOK, strlen(httpOK));

                        Send(c, data, size);
                    }
                    else // 404 NOT FOUND
                    {
                    }
                    fclose(f);
                }
            }
        }
        else if (strcmp(method, "POST") == 0)
        {
        }
    }
    close(c);
    return NULL;
}

int main()
{
    strcpy(currentPath, "/home/huytr");
    sockaddr_in myaddr, caddr;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(5000);
    myaddr.sin_addr.s_addr = 0;
    int clen = sizeof(caddr);

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int error = bind(s, (sockaddr *)&myaddr, sizeof(sockaddr));
    if (error == 0)
    {
        listen(s, 10);
        while (true)
        {
            int c = accept(s, (sockaddr *)&caddr, (socklen_t *)&clen);
            pthread_t tid;
            int *arg = (int *)calloc(1, sizeof(int));
            *arg = c;
            pthread_create(&tid, NULL, processThread, (void *)arg);
        }
    }
    else
        cout << "Bind error! Check your port!\n";
}
