#include <algorithm>
#include "pendingpool.h"

PendingPool::PendingPool(uint cpct): _cond(PTHREAD_COND_INITIALIZER), _mutex(PTHREAD_MUTEX_INITIALIZER), _data(nullptr), _status(nullptr), _capacity(cpct), _size(0), _next(0) {
    _capacity = std::max(MIN_PEND_SIZ, _capacity);
    _capacity = std::min(MAX_PEND_SIZ, _capacity);
    _data = new int[_capacity];
    _status = new SocketStatus[_capacity];
    for (uint i = 0; i < _capacity; ++i) {
        _status[i] = SocketStatus::CLOSE;
    }
}

int PendingPool::put(int fd) {
    int s = pthread_mutex_lock(&_mutex);
    if (s != 0) { // error
        return -1;
    }
    while (_size == _capacity) {
        s = pthread_cond_wait(&_cond, &_mutex);
        if (s != 0) {
            return -1;
        }
    }
    s = do_put(fd);
    if (pthread_mutex_unlock(&_mutex) != 0) {
        return -1;
    }
    if (pthread_cond_broadcast(&_cond) != 0) {
        return -1;
    }
    return s;
}

int PendingPool::do_put(int fd) {
    for (uint i = _next; i != _next; i = (i + 1) % _capacity) {
        if (_status[i] == SocketStatus::CLOSE) {
            _data[i] = fd;
            _status[i] = SocketStatus::PENDING;
            ++_size;
            _next = i;
            return 0;
        }
    }
    return -1;
}

int PendingPool::get() {
    int s = pthread_mutex_lock(&_mutex);
    if (s != 0) {
        return -1;
    }
    while (_size == _capacity) {
        s = pthread_cond_wait(&_cond, &_mutex);
        if (s != 0) {
            return -1;
        }
    }
    s = do_get();
    if (pthread_mutex_unlock(&_mutex) != 0) {
        return -1;
    }
    if (pthread_cond_broadcast(&_cond) != 0) {
        return -1;
    }
    return s;
}

int PendingPool::do_get() {
    for (uint i = _next; i != _next; i = (i + 1) % _capacity) {
        if (_status[i] == SocketStatus::PENDING) {
            _status[i] = SocketStatus::USING;
            _next = i;
            return _data[i];
        }
    }
    return -1;
}

int PendingPool::release(int fd) {
    int s = pthread_mutex_lock(&_mutex);
    if (s != 0) {
        return -1;
    }
    s = do_release(fd);
    if (pthread_mutex_unlock(&_mutex) != 0) {
        return -1;
    }
    if (pthread_cond_broadcast(&_cond) != 0) {
        return -1;
    }
    return s;
}

int PendingPool::do_release(int fd) {
    for (uint i = _next; i != _next; i = (i + 1) % _capacity) {
        if (_data[i] == fd) {
            _status[i] = SocketStatus::CLOSE;
            --_size;
            return 0;
        }
    }
    return -1;
}


