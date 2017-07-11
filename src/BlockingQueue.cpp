#include <stdexcept>
#include <pthread.h>
#include "BlockingQueue.h"

BlockingQueue::BlockingQueue(uint size): _mutex(PTHREAD_MUTEX_INITIALIZER), _cond(PTHREAD_COND_INITIALIZER), _queue{nullptr}, _begin{0}, _end{0}, _size{size} {

    if (_size < MIN_QUEUE_SIZE) {
        _size = MIN_QUEUE_SIZE;
    } else if (_size > MAX_QUEUE_SIZE) {
        _size = MAX_QUEUE_SIZE;
    }
    _queue = new int[_size];
}

int BlockingQueue::get(int *val) {

    if (val == nullptr) {
        return -1;
    }
    if (pthread_mutex_lock(&_mutex) != 0) {
        return -1;
    }
    while (is_empty()) {
        int res = pthread_cond_wait(&_cond, &_mutex);
        if (res != 0) {
            return -1;
        }
    }
    *val = _queue[_begin];
    _begin = (_begin + 1) % _size;
    if (pthread_mutex_unlock(&_mutex) != 0) {
        return -1;
    }
    if (pthread_cond_broadcast(&_cond) != 0) {
        return -1;
    }
    return 0;
}

int BlockingQueue::put(int val) {

    if (pthread_mutex_lock(&_mutex) != 0) {
        return -1;
    }
    while (is_full()) {
        int res = pthread_cond_wait(&_cond, &_mutex);
        if (res != 0) {
            return -1;
        }
    }
    _queue[_end] = val;
    _end = (_end + 1) % _size;
    if (pthread_mutex_unlock(&_mutex) != 0) {
        return -1;
    }
    if (pthread_cond_broadcast(&_cond) != 0) {
        return -1;
    }
    return 0;
}




