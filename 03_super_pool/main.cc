// standard headers
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

// OpenMP functions
#include <omp.h>

// our headers
#include "thread_pool.hh"

///*

Thread_pool __cpu_pool(1);
Thread_pool __io_pool(1);

/// Stops all pools at once.
void stop_pools()
{
    __cpu_pool.stop();
    __io_pool.stop();
}

Super_thread_pool sthp(4);

struct Hello_actor: public Actor {
    void act() {
        #pragma omp parallel
        {
            int rank = omp_get_thread_num();
            int nthreads = omp_get_num_threads();
            for (int i = 0; i < 1e8; ++i) {}
            std::stringstream msg;
            msg << "Hello from " << rank+1 << '/' << nthreads << std::endl;
            //std::cout << msg.str();
        }
        //std::cout << std::endl;
        stop_pools();
	}
};
//*/

int main()
{
    for(int i = 0; i < 10; ++i)
    {
        __cpu_pool.submit(new Hello_actor);
        sthp.submit(new Hello_actor);
    }

    //stop_pools();
    __cpu_pool.wait();
    __io_pool.wait();

    for (int i = 0; i < 1e8; ++i) {}
    sthp.stop();
    sthp.wait();
// return 0;
}
