#include <malloc.h>
#include <vector>
#include <bits/utility.h>
#include <thread>
#include <string>
#include <iostream>
#include "server.h"
#include "skiplist.h"

#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <time.h>


#define NUM_THREADS 1
#define TEST_COUNT 100000

Skiplist<int, std::string> skipList(18);

void *insertElement(void* threadid) {
    long tid;
    tid = (long)threadid;
    std::cout << tid << std::endl;
    int tmp = TEST_COUNT/NUM_THREADS;
    for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
        skipList.insert_element(rand() % TEST_COUNT, "a");
    }
    pthread_exit(nullptr);
}

void *getElement(void* threadid) {
    long tid;
    tid = (long)threadid;
    std::cout << tid << std::endl;
    int tmp = TEST_COUNT/NUM_THREADS;
    for (int i=tid*tmp, count=0; count<tmp; i++) {
        count++;
        std::string ch;
        skipList.search_element(rand() % TEST_COUNT, ch);
    }
    pthread_exit(0);
}

void stress_test()
{
    srand (time(0));
    {

        pthread_t threads[NUM_THREADS];
        int rc;
        int i;

        auto start = std::chrono::high_resolution_clock::now();

        for( i = 0; i < NUM_THREADS; i++ ) {
            std::cout << "main() : creating thread, " << i << std::endl;
            rc = pthread_create(&threads[i], nullptr, insertElement, (void *)i);

            if (rc) {
                std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        void *ret;
        for( i = 0; i < NUM_THREADS; i++ ) {
            if (pthread_join(threads[i], &ret) !=0 )  {
                perror("pthread_create() error");
                exit(3);
            }
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "insert elapsed:" << elapsed.count() << std::endl;
    }
    // skipList.displayList();

    // {
    //     pthread_t threads[NUM_THREADS];
    //     int rc;
    //     int i;
    //     auto start = std::chrono::high_resolution_clock::now();

    //     for( i = 0; i < NUM_THREADS; i++ ) {
    //         std::cout << "main() : creating thread, " << i << std::endl;
    //         rc = pthread_create(&threads[i], NULL, getElement, (void *)i);

    //         if (rc) {
    //             std::cout << "Error:unable to create thread," << rc << std::endl;
    //             exit(-1);
    //         }
    //     }

    //     void *ret;
    //     for( i = 0; i < NUM_THREADS; i++ ) {
    //         if (pthread_join(threads[i], &ret) !=0 )  {
    //             perror("pthread_create() error");
    //             exit(3);
    //         }
    //     }

    //     auto finish = std::chrono::high_resolution_clock::now();
    //     std::chrono::duration<double> elapsed = finish - start;
    //     std::cout << "get elapsed:" << elapsed.count() << std::endl;
    // }

    pthread_exit(0);
}
int main() {
    Server server(10);
    server.start();
    return 0;

}

