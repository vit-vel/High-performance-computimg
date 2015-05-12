#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

struct Actor
{
    virtual void act() = 0;
};

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
                Actor* task = actors.front();
                actors.pop();

                locker.unlock();
                std::cout << std::this_thread::get_id() << std::endl;
                // std::cout << stop_flag << std::endl;
                task->act();
                locker.lock();
            }
        }
    }
};

Thread_pool __cpu_pool(1);
Thread_pool __io_pool(1);

/// Stops all pools at once.
void stop_pools()
{
    __cpu_pool.stop();
    __io_pool.stop();
}



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
    {
        for (int i = 0; i < nthreads; ++i)
        {
            pools.push_back(new Thread_pool(1));
        }
    }

    /// Places actor @a to execution queue.
    void submit(Actor *a)
    {
        std::unique_lock<Spin_mutex> locker(smutex);
        int i  = rand() % pools.size();
        // распределение, результат в i
        pools[i]->submit(a);
    }

    /// Waits for all threads to finish.
    void wait()
    {
        std::unique_lock<Spin_mutex> locker(smutex);
        for (auto pool: pools)
        {
            pool->wait();
        }
    }

    /// Stops processing of actors.
    void stop()
    {
        std::unique_lock<Spin_mutex> locker(smutex);
        for (auto pool: pools)
        {
            pool->stop();
        }
    }

private:
    std::vector<Thread_pool*> pools;
    Spin_mutex smutex;

};