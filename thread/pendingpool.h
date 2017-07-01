#ifndef _PENDING_POOL_H_
#define _PENDING_POOL_H_

#include <pthread.h>

const unsigned int MIN_PEND_SIZ = 1024;
const unsigned int MAX_PEND_SIZ = 102400;

enum class SocketStatus {
    CLOSE, //not connected
    PENDING, //connected but not using
    USING //connected and using
};

class PendingPool {
    using uint = unsigned int;
    public:
        explicit PendingPool(uint cpct);
        PendingPool(const PendingPool &) = delete;
        PendingPool(PendingPool &&) = delete;
        PendingPool& operator=(const PendingPool &) = delete;
        PendingPool& operator=(PendingPool &&) = delete;
        int put(int fd);
        int release(int fd);
        int get();
        uint size() const { return _size; }
        uint capacity() const { return _capacity; }
    private:
        pthread_cond_t _cond;
        pthread_mutex_t _mutex;
        int *_data;
        SocketStatus *_status;
        uint _capacity;
        uint _size;
        uint _next;
        int do_put(int fd);
        int do_get();
        int do_release(int fd);
};


#endif


