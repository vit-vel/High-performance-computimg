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

/*

Thread_pool __cpu_pool(1);
Thread_pool __io_pool(1);

/// Stops all pools at once.
void stop_pools()
{
    __cpu_pool.stop();
    __io_pool.stop();
}

 // */


struct Hello_actor: public Actor {

    Hello_actor(Actor *parent = nullptr, size_t number_of_child = 0)
            : Actor(parent)
            , child_count(number_of_child)
    {}

    virtual void act() {
        //global_pool.submit(new Hello_actor2(this));
        std::cout << "Hello from Hello_actor" << std::endl;
        //try_done();
    }

    void try_done()
    {
        if (!child_count)
        {
            done();
        }
    }

    void react(Actor *child)
    {
        --child_count;
        try_done();
    }

    void run()
    {
        if (is_done() && get_parent())
        {
            get_parent()->react(this);
            if (get_parent()->is_done())
            {
                global_pool.submit(get_parent());
            }
            return;
        }

        if (!is_done())
        {
            act();
            if(is_done())
            {
                //std::cout << "react is_done == true" << std::endl;
                global_pool.submit(this);
            }
        }
    }

private:
    size_t child_count;
};


struct Hello_actor_2 : Hello_actor
{
    Hello_actor_2(Actor *parent = nullptr) : Hello_actor(parent)
    {}

    void act()
    {
        std::cout << "Hello from Hello_parent_2" << std::endl;
        try_done();
    }
};

struct Hello_actor_1 : Hello_actor
{
    Hello_actor_1(Actor *parent = nullptr) : Hello_actor(parent, 1)
    {}

    void act()
    {
        global_pool.submit(new Hello_actor_2);
        std::cout << "Hello from Hello_parent_1" << std::endl;
        try_done();
    }
};


//*/

int main()
{
    for(int i = 0; i < 2; ++i)
    {
        //__cpu_pool.submit(new Hello_actor);
        global_pool.submit(new Hello_actor_1);
    }

    //stop_pools();
    //__cpu_pool.wait();
    //__io_pool.wait();

    for (int i = 0; i < 1e8; ++i) {}
    global_pool.stop();
    global_pool.wait();
// return 0;
}