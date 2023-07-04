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

/*
**  Create file HTML
*/

#include <malloc.h>
#include <dirent.h>
char *html = NULL;

using namespace std;
typedef sockaddr sockaddr;
typedef sockaddr_in sockaddr_in;

vector<int> client_list;

void Append(char **phtml, const char *name)
{
    int oldLen = *phtml == NULL ? 0 : strlen(*phtml);
    (*phtml) = (char *)realloc(*phtml, oldLen + strlen(name) + 1);
    memset(*phtml + oldLen, 0, strlen(name) + 1);
    sprintf(*phtml + oldLen, "%s", name);
}

void createHTTP()
{
    struct dirent **namelist;
    int n;
    n = scandir(".", &namelist, NULL, NULL);
    if (n == -1)
    {
        printf("Error\n");
    }
    else
    {
        Append(&html, "HTTP/1.1 200 OK\r\n");
        Append(&html, "Content-Type: text/html\r\r\n");
        Append(&html, "<html>\n");
        while (n--)
        {
            Append(&html, "<a href = \"");
            Append(&html, namelist[n]->d_name);
            if (namelist[n]->d_type == DT_DIR)
            {
                Append(&html, "\"><b>");
                Append(&html, namelist[n]->d_name);
                Append(&html, "</b></a><br>\n");
            }
            else
            {
                Append(&html, "\"><i>");
                Append(&html, namelist[n]->d_name);
                Append(&html, "</i></a><br>\n");
            }
            free(namelist[n]);
        }
        Append(&html, "</html>\n");
        free(namelist);
        namelist = NULL;
        FILE *f = fopen("scandir.html", "wt");
        fputs(html, f);
        fclose(f);
        free(html);
        html = NULL;
    }
}

/*
**  Process Thread
*/
void *processThread(void *arg)
{
    int c = *((int *)arg);
    client_list.push_back(c);
    printf("Client %d connect!\n", c);
    free(arg);
    arg = NULL;

    // process
    createHTTP();
    FILE *f = fopen("scandir.html", "rb");
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *http = (char *)calloc(length + 1, 1);
    fread(http, 1, length, f);
    fclose(f);
    cout << http << endl;
    send(c, http, strlen(http), 0);
    close(c);
    free(http);
    http = NULL;
    return NULL;
}

int main()
{
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
            pthread_create(&tid, NULL, processThread, arg);
        }
    }
    else
        cout << "Bind error! Please check your port!\n";
}
