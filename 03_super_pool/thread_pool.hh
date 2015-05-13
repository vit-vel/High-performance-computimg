#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <functional>

enum struct Result
{
    UNDEFINED,
    DONE
};

struct Actor
{
    Actor(Actor *parent = nullptr) : parent_(parent)
    {}

    virtual void act() = 0;
    virtual void react(Actor *child) = 0;
    virtual void run() = 0;

    void done()
    {
        result = Result::DONE;
    }

    bool is_done()
    {
        return result == Result::DONE;
    }

    Actor* get_parent()
    {
        return parent_;
    }

private:
    Actor *parent_;
    Result result = Result::UNDEFINED;

};


std::hash<Actor *> actor_hash;

struct Thread_pool
{

    /// Creates a pool with @nthreads number of threads.
    explicit Thread_pool(int nthreads = std::thread::hardware_concurrency())
            : stop_flag(false), actors()
    {
        for (int i = 0; i < nthreads; ++i)
        {
            threads.push_back(std::thread(&Thread_pool::thread_fn, this));
        }
    }

    /// Places actor @a to execution queue.
    void submit(Actor* a)
    {
        std::unique_lock<std::mutex> locker(mutex);
        actors.push(a);
        condition.notify_one();
    }

    /// Waits for all threads to finish.
    void wait()
    {
        for (size_t i = 0; i < threads.size(); ++i)
        {
            threads[i].join();
        }
    }

    /// Stops processing of actors.
    void stop()
    {
        if (!stop_flag)
        {
            std::unique_lock<std::mutex> locker(mutex);
            stop_flag = true;
            condition.notify_all();
        }
    }

private:
    bool stop_flag;
    std::queue<Actor*> actors;
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::condition_variable condition;

    void thread_fn()
    {
        while (!stop_flag)
        {
            std::unique_lock<std::mutex> locker(mutex);

            condition.wait(locker, [&](){ return !actors.empty() || stop_flag; });

            while (!actors.empty() && !stop_flag)
            {
                Actor* act = actors.front();
                actors.pop();

                locker.unlock();
                // std::cout << std::this_thread::get_id() << std::endl;
                // std::cout << stop_flag << std::endl;
                act->run();
                locker.lock();
            }
        }
    }
};

struct Spin_mutex
{
    void lock()
    {
        while (f.test_and_set());
    }

    void unlock()
    {
        f.clear ();
    }

private:
    std::atomic_flag f = ATOMIC_FLAG_INIT ;
};

struct Super_thread_pool
{
    /// Creates a pool with @nthreads number of threads.
    explicit Super_thread_pool(int nthreads
    = std::thread::hardware_concurrency())
            : count(0)
    {
        for (int i = 0; i < nthreads; ++i)
        {
            pools.push_back(new Thread_pool(1));
        }
    }

    /// Places actor @a to execution queue.
    void submit(Actor *a)
    {
        std::unique_lock<Spin_mutex> locker(s_mutex);
        // assignment
        int i;
        if (a->get_parent())
        {
            i = actor_hash(a->get_parent()) % 101;
        } else
        {
            i = count++;
        }
        i = i % pools.size();
        pools[i]->submit(a);
    }

    /// Waits for all threads to finish.
    void wait()
    {
        for (auto pool: pools)
        {
            pool->wait();
        }
    }

    /// Stops processing of actors.
    void stop()
    {
        for (auto pool: pools)
        {
            pool->stop();
        }
    }

private:
    std::vector<Thread_pool*>   pools;
    Spin_mutex                  s_mutex;
    unsigned short              count;

};

Super_thread_pool global_pool(4);