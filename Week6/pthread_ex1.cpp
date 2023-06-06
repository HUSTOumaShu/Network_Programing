#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

void *mythread(void *arg)
{
    int i = *((int *)arg);
    free(arg);
    cout << "from child thread: " << i << ": " << pthread_self() << endl;
    return 0;
}

int main()
{
    int *status = NULL;
    pthread_t tid;
    for (int i = 0; i < 10; i++)
    {
        int *arg = (int *)calloc(1, sizeof(int));
        *arg = i;
        // int arg = i;
        pthread_create(&tid, NULL, mythread, arg);
    }

    cout << "from parent thread!\n";
    getchar();
}
