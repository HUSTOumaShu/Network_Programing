#include <stdio.h>
#include <dirent.h>
#include <malloc.h>
#include <string.h>
char *html = NULL;

void Append(char **phtml, const char *name)
{
    int oldLen = *phtml == NULL ? 0 : strlen(*phtml);
    (*phtml) = (char *)realloc(*phtml, oldLen + strlen(name) + 1);
    memset(*phtml + oldLen, 0, strlen(name) + 1);
    sprintf(*phtml + oldLen, "%s", name);
}

int main()
{
    struct dirent **namelist;
    int n;
    n = scandir("/home/huytr", &namelist, NULL, NULL);
    if (n == -1)
    {
        printf("Error\n");
    }
    else
    {
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
