#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

int S1 = 0, S2 = 0;
int N = 100;
pthread_mutex_t *mutex = NULL;

void *addOne(void *arg)
{
    int i = *((int *)arg);
    free(arg);
    arg = NULL;

    // Xu ly source co kha nang xay ra xung dot
    pthread_mutex_lock(mutex);
    S2 += i;
    pthread_mutex_unlock(mutex);
    //********

    return 0;
}

int main()
{
    // Xu ly 1 luong
    for (int i = 0; i < N; i++)
        S1 += i;

    // Xu ly da luong
    mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);

    pthread_t *pid = (pthread_t *)calloc(100, sizeof(pthread_t));
    for (int i = 0; i < N; i++)
    {
        int *arg = (int *)calloc(1, sizeof(int));
        *arg = i;
        pthread_create(pid, NULL, addOne, arg);
    }
    // Xu ly xung dot thoi gian ket thuc luong
    int *status = NULL;
    for (int i = 0; i < N; i++)
    {
        pthread_join(pid[i], (void **)&status);
    }

    // Don dep tai nguyen
    free(pid);
    pid = NULL;
    pthread_mutex_destroy(mutex);
    free(mutex);
    cout << S1 << "=" << S2 << endl;
}
