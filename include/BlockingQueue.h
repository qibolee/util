#ifndef _BLOCKING_QUEUE_H_
#define _BLOCKING_QUEUE_H_

#include <mutex>
#include <condition_variable>

// min queue capacity
const unsigned int MIN_QUEUE_SIZE = 8;

// max queue capacity
const unsigned int MAX_QUEUE_SIZE = 102400;

template<typename T>
class BlockingQueue {
    using uint = unsigned int;

public:
    /**
     * constructor with capacity
     */
    BlockingQueue(uint size = 1024);

    BlockingQueue(const BlockingQueue &bq) = delete;
    BlockingQueue(BlockingQueue &&bq) = delete;
    BlockingQueue &operator=(const BlockingQueue &bq) = delete;
    BlockingQueue &operator=(BlockingQueue &&bq) = delete;

    /**
     * destroy
     */
    ~BlockingQueue();

    /**
     * get a num, put in val
     * return 0 on success, or -1 if error occurred
     */
    int get(T &val);

    /**
     * put a num by val
     * return 0 on success, or -1 if error occurred
     */
    int put(const T &val);

    /**
     * get queue capacity
     */
    uint capacity() const { return _size; }

    /**
     * get queue size
     */
    uint size() const { return _end >= _begin ? _end - _begin : _end - _begin + _size; }

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    T *_queue;
    uint _begin;
    uint _end;
    uint _size;
    bool is_empty() const { return _begin == _end; }
    bool is_full() const { return ((_end + 1) % _size) == _begin; }
};

template<typename T>
BlockingQueue<T>::BlockingQueue(uint size): _mutex{}, _cond{}, _queue{nullptr}, _begin{0}, _end{0}, _size{size} {

    if (_size < MIN_QUEUE_SIZE) {
        _size = MIN_QUEUE_SIZE;
    } else if (_size > MAX_QUEUE_SIZE) {
        _size = MAX_QUEUE_SIZE;
    }
    _queue = new T[_size];
}

template<typename T>
int BlockingQueue<T>::get(T &val) {

    std::unique_lock <std::mutex> lck(_mutex);
    while (is_empty()) {
        _cond.wait(lck);
    }
    val = _queue[_begin];
    _begin = (_begin + 1) % _size;
    _cond.notify_all();
    
    return 0;
}

template<typename T>
int BlockingQueue<T>::put(const T &val) {

    std::unique_lock <std::mutex> lck(_mutex);
    while (is_full()) {
        _cond.wait(lck);
    }
    _queue[_size] = val;
    _begin = (_begin + 1) % _size;
    _cond.notify_all();
    
    return 0;
}

template<typename T>
BlockingQueue<T>::~BlockingQueue() {
    delete [] _queue;
}


#endif
