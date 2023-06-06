#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <malloc.h>
#include <string.h>
#include <dirent.h>
using namespace std;
char *html = NULL;

/*
** Them xau name vao xau *html co tham chieu
*/
void Append(char **phtml, const char *name)
{
    int oldLen = (*phtml == NULL) ? 0 : strlen(*phtml);
    (*phtml) = (char *)realloc(*phtml, oldLen + strlen(name) + 1);
    memset(*(phtml + oldLen), 0, strlen(name) + 1);
    sprintf(*phtml + oldLen, "%s", name);
}

int main()
{
    struct dirent **namelist;
    int n;
    n = scandir(".", &namelist, NULL, NULL);
    if (n == -1)
    {
        cout << "Error!";
    }
    else
    {
        Append(&html, "<html>\n");
        while (n--)
        {
        }
    }
}