#include <iostream>
//#include <iomanip>
#include <sstream>
//#include <thread>
//#include <atomic>
#include <cassert>
#include <time.h>

#include "thread_pool.hh"
//#include "sync_threads.hh"
#include "map_reduce.hh"

int main() {

	// Thread_pool::run test
    Thread_pool pool(4);
	pool.run([] (int rank, int nthreads) {
		std::stringstream msg;
		msg << "Hello from " << rank << '/' << nthreads << '\n';
		std::cout << msg.str();
	});
	pool.wait();


	// map & reduce test
	for(int i = 1; i < 1000; ++i)
	{
		std::vector<int> v(i, 1);
		map(v, [](int x){return x * 2;});
		reduce(v, [](int x, int y){ return x + y;});
		assert(v[0] == i*2);
	}
	// std::vector<double> v(134217728, 1);
	// std::cout << mean(v) << std::endl;
	// std::cout << variance(v) << std::endl;

	// mean & variance test
	clock_t t_par;
	clock_t t_seq;
	time_t t;

	int MAX_DIMENSION = 1000000000;

	std::vector<float> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::vector<float> v2(v);
	std::cout << mean(v) << std::endl;
	std::cout << variance(v2) << std::endl;

	// mean test
	 /*
	std::cout << "mean:" << std::endl;
	for(int i = 10000; i < MAX_DIMENSION; i *= 10)
	{
		// generation of vector with vector.size = i
		std::vector<float> v(i);
		for(int j = 0 ; j < i; ++j)
		{
			v[j] = rand() / float(RAND_MAX);
		}

		time(&t);
		t_seq = clock(); // sequential variant start
		for(int j = 0; j < MAX_DIMENSION  / i; ++j)
		{
			float result = 0;
			for(auto value : v)
			{
				result += value;
			}
			result /= v.size();
		}
		t_seq = clock() - t_seq;
		float time1 = time(NULL) - t;

		time(&t);
		t_par = clock(); // parallel variant start
		for(int j = 0; j < MAX_DIMENSION / i; ++j)
		{
			mean(v);
		}
		t_par = clock() - t_par; // parallel variant stop
		float time2 = time(NULL) - t;

		std::cout << "dimension: " << i << "\t\t";
		std::cout << float(t_seq) / CLOCKS_PER_SEC << "\t\t";
		std::cout << float(t_par) / CLOCKS_PER_SEC << std::endl;
		std::cout << "dimension: " << i << "\t\t";
		std::cout << time1 << "\t\t";
		std::cout << time2 << std::endl;

	}
	// */
	// for 2 threads on my maschine
	// result of mean test: parallel mean is better when dimension > 100000

	// for 4 threads on my maschine ( thread::hardware_concurrency() == 4)
	// result of mean test: parallel mean is better when dimension > 1000000

	// for 8 threads on my maschine
	// result of mean test: parallel mean is better when dimension > 1000000


	// variance test

	MAX_DIMENSION = 1000000000;
	 /*
	std::cout << std::endl << "variance" << std::endl;
	for(int i = 1000000; i < MAX_DIMENSION; i *= 10)
	{
		// generation of vector with vector.size = i
		std::vector<float> v(i);
		for(int j = 0 ; j < i; ++j)
		{
			v[j] = rand() / float(RAND_MAX);
		}

		time(&t);
		t_seq = clock(); // sequential variant start
		for(int j = 0; j < MAX_DIMENSION  / i / 10; ++j)
		{
			float m = 0;
			for(auto value : v)
			{
				m += value;
			}
			m /= v.size(); // mean

			float result = 0;
			for(auto value : v)
			{
				float tmp = m - value;
				result += tmp * tmp;
			}

			result /= v.size();
		}
		t_seq = clock() - t_seq;
		float time1 = time(NULL) - t;

		time(&t);
		t_par = clock(); // parallel variant start
		for(int j = 0; j < MAX_DIMENSION / i / 10; ++j)
		{
			variance(v);
		}
		t_par = clock() - t_par; // parallel variant stop
		float time2 = time(NULL) - t;

		std::cout << "dimension: " << i << "\t\t";
		std::cout << float(t_seq) / CLOCKS_PER_SEC << "\t\t";
		std::cout << float(t_par) / CLOCKS_PER_SEC << std::endl;
		std::cout << "dimension: " << i << "\t\t";
		std::cout << time1 << "\t\t";
		std::cout << time2 << std::endl;

	}
	//*/
	// in variance I have to copy container. Therefore results not good

	// for 2 threads on my maschine
	// result of variance test: parallel variance is always worse

	// for 4 threads on my maschine ( thread::hardware_concurrency() == 4)
	// result of variance test: parallel variance is always worse

	// for 8 threads on my maschine
	// result of variance test: parallel variance is always worse




	return 0;
}
