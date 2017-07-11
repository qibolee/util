#include <cstdio>
#include <thread>
#include "BlockingQueue.h"

BlockingQueue<int> queue;

void product() {

    int val = 12;
    for (int i = 0; i < 20; ++i) {    
        queue.put(val);
    }
}

void consume() {

    int val;
    for (int i = 0; i < 20; ++i) {    
        queue.get(val);
    }
}

int main(int argc, const char *argv[]) {

    std::thread threads[20];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(product);
    }
    for (int i = 10; i < 20; ++i) {
        threads[i] = std::thread(consume);
    }

    for (auto &th : threads) {
        th.join();
    }
    printf("done\n");

    return 0;
}
