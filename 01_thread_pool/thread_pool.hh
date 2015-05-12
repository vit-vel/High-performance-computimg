#ifndef  THREAD_POOL_HH
#define THREAD_POOL_HH

#include <thread>
#include <vector>

struct Thread_pool {

	/// Creates a poll with @nthreads number of threads.
	explicit Thread_pool(int nthreads)
	{
		threads.resize(nthreads);
	}

	/// Calls function @f in each thread passing
	/// thread number and total number of threads as arguments:
	/// f(thread_no, nthreads)
	template<class Func>
	void run(Func f)
	{
		size_t n = threads.size();
		for(size_t i = 0; i < n; ++i)
		{
			threads[i] = std::thread(f, i, n);
		}
	}

	/// Waits for all threads to finish.
	void wait()
	{
	// needs in range-based for and check on finishing threads
        for(size_t i = 0; i < threads.size(); ++i)
		{
			threads[i].join();
		}
	}

private:
	std::vector<std::thread> threads;
};

#endif
