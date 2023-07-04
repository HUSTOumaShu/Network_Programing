#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;

int main()
{
    int day, year;
    char method[20] = {0}, path[20] = {0}, ver[100] = {0}, dtm[100] = {0};

    strcpy(dtm, "GET / HTTP:/1.1");
    sscanf(dtm, "%s%s%s", method, path, ver);

    printf("%s %s %s\n", method, path, ver);

    return (0);
}
