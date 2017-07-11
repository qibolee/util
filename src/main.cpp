#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "BlockingQueue.h"

static pthread_cond_t dead_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t dead_mutex = PTHREAD_MUTEX_INITIALIZER;
static int dead_num = 0;
static BlockingQueue *bq = nullptr;

static void *productThread(void *arg) {
    
    for (int i = 0; i < 10; ++i) {
        if (bq == nullptr) {
            break;
        }
        int val = 12;
        if (bq->put(val) == -1) {
            break;
        }
    }
    if (pthread_mutex_lock(&dead_mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    ++dead_num;
    if (pthread_mutex_unlock(&dead_mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    if (pthread_cond_broadcast(&dead_cond) != 0) {
        perror("pthread_cond_broadcast");
        exit(1);
    }
    
    return nullptr;
}


static void *consumeThread(void *arg) {
    
    for (int i = 0; i < 10; ++i) {
        if (bq == nullptr) {
            break;
        }
        int val;
        if (bq->get(&val) == -1) {
            break;
        }
    }
    if (pthread_mutex_lock(&dead_mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    ++dead_num;
    if (pthread_mutex_unlock(&dead_mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    if (pthread_cond_broadcast(&dead_cond) != 0) {
        perror("pthread_cond_broadcast");
        exit(1);
    }
    
    return nullptr;
}

int main(int argc, const char *argv[]) {

    bq = new BlockingQueue(100);
    int val = 12;
    if (bq->put(val) == -1) {
        perror("BlockingQueue::put");
        exit(1);
    }
    int dat;
    if (bq->get(&dat) == -1) {
        perror("BlockingQueue::get");
        exit(1);
    }
    printf("%d\n", dat);
    
    int thread_num = 0;
    for (int i = 0; i < 1; ++i) {
        if (pthread_create(NULL, NULL, productThread, NULL) == 0) {
            ++thread_num;
        } else {
            perror("pthread_create");
        }
    }
    for (int i = 0; i < 1; ++i) {
        if (pthread_create(NULL, NULL, consumeThread, NULL) == 0) {
            ++thread_num;
        } else {
            perror("pthread_create");
        }
    }

    if (pthread_mutex_lock(&dead_mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    while (++dead_num < thread_num) {
        int res = pthread_cond_wait(&dead_cond, &dead_mutex);
        if (res != 0) {
            perror("pthread_cond_wait");
            exit(1);
        }
    }
    if (pthread_mutex_unlock(&dead_mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    
    return 0;
}
