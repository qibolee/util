#ifndef _BLOCKING_QUEUE_H_
#define _BLOCKING_QUEUE_H_

#include <pthread.h>

// min queue capacity
const unsigned int MIN_QUEUE_SIZE = 8;

// max queue capacity
const unsigned int MAX_QUEUE_SIZE = 102400;

class BlockingQueue {
    using uint = unsigned int;

public:
    // constructor with capacity
    BlockingQueue(uint size);

    BlockingQueue(const BlockingQueue &bq) = delete;
    BlockingQueue(BlockingQueue &&bq) = delete;
    BlockingQueue &operator=(const BlockingQueue &bq) = delete;
    BlockingQueue &operator=(BlockingQueue &&bq) = delete;

    /**
     * get a num, put in val
     * return 0 on success, or -1 if error
     */
    int get(int *val);

    /**
     * put a num by val
     * return 0 on success, or -1 if error
     */
    int put(int val);

    // get queue capacity
    uint capacity() const { return _size; }

private:
    pthread_mutex_t _mutex;
    pthread_cond_t _cond;
    int *_queue;
    uint _begin;
    uint _end;
    uint _size;
    bool is_empty() const { return _begin == _end; }
    bool is_full() const { return ((_end + 1) % _size) == _begin; }
};


#endif
