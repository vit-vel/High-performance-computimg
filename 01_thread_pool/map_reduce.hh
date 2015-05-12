#ifndef MAP_REDUCE_HH
#define MAP_REDUCE_HH

#include "thread_pool.hh"

int nthreads =  std::thread::hardware_concurrency();

template<class Container, class FuncT>
void map(Container& cnt, FuncT f) {
	// f takes exactly one argument
	Thread_pool pool(nthreads);
	pool.run([&cnt,f] (int rank, int nthreads) {
		int start = cnt.size() / nthreads;
		int end = ( (rank + 1) == nthreads ? cnt.size() : start * (rank + 1) );
		start *= rank;

		for (;start < end; ++start)
		{
			cnt[start] = f(cnt[start]);
		}
	});
	pool.wait();
}

template<class Container, class Func>
void reduce(Container& cnt, Func f) {
	//  f takes exactly two argument and must be associative
	// result wil be write in cnt[0]

	int cnt_size = cnt.size();
	int length_of_part = cnt_size / nthreads;

	if(cnt_size < 2)
	{
		return;
	} else if (cnt_size < nthreads)
	{
		for(int i = 1; i < cnt_size; ++i)
		{
			cnt[0] = f(cnt[0], cnt[i]);
		}
		return;
	}

	Thread_pool pool(nthreads);
	pool.run([&cnt, cnt_size, length_of_part, f] (int rank, int nthreads) {

		int start = length_of_part * (rank++);
		// if this is the last thread then take part until to the end of container
		int end = rank == nthreads ? cnt_size : length_of_part * rank;

		for(int i = start + 1; i < end ; ++i)
		{
			cnt[start] = f(cnt[start], cnt[i]);
		}
	});
	pool.wait();

	for(int i = 1; i < nthreads; ++i)
	{
		cnt[0] += cnt[i * length_of_part];
	}
}

template<class Container>
float mean(Container& cnt)
{
	// result in cnt[0]
	typedef typename Container::value_type t;
	reduce(cnt, [](t x, t y){ return x + y;});
	return float(cnt[0]) / cnt.size();
}

template<class Container>
void print(Container& cnt)
{
	for( auto value : cnt)
	{
		std::cout << value << " ";
	}
	std:: cout << std::endl;
}

template<class Container>
float variance(Container& cnt)
{
	// result in cnt[0]
	typedef typename Container::value_type t;

	Container cnt_copy(cnt);
	float m = mean(cnt_copy);

	map(cnt, [m](t x){ return m - x; });
	map(cnt, [](t x){ return x * x; });
	reduce(cnt, [](t x, t y){ return x + y;});
	return float(cnt[0]) / cnt.size();
}



#endif