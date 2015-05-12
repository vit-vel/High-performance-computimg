#include <iostream>
#include <iomanip>
#include <sstream>
#include <atomic>
#include <omp.h>
#include <vector>
#include "mean_var.hh"

int main() {

	#pragma omp parallel
	{
		int rank = omp_get_thread_num();
		int nthreads = omp_get_num_threads();
		std::stringstream msg;
		msg << "Hello from " << rank+1 << '/' << nthreads << std::endl;
		std::cout << msg.str();
	}

	clock_t t_par;
	clock_t t_seq;
	time_t t;

	int MAX_DIMENSION = 1000000000;

	std::vector<double> v(234217728, 1.);
	std::cout << mean(v) << std::endl;
	std::cout << variance(v) << std::endl;

	// mean test
	// /*
	std::cout << "mean:" << std::endl;
	for(int i = 10000; i < MAX_DIMENSION; i *= 10)
	{
		// generation of vector with vector.size = i
		std::vector<float> v(i);
		for(int j = 0 ; j < i; ++j)
		{
			v[j] = rand() / float(RAND_MAX);
		}

		omp_set_num_threads(1);
		time(&t);
		t_seq = clock(); // sequential variant start
		for(int j = 0; j < MAX_DIMENSION  / i; ++j)
		{
			mean(v);
		}
		std::cout << mean(v) << std::endl;
		t_seq = clock() - t_seq;
		float time1 = time(NULL) - t;

		omp_set_num_threads(2);
		time(&t);
		t_par = clock(); // parallel variant start
		for(int j = 0; j < MAX_DIMENSION / i; ++j)
		{
			mean(v);
		}
		std::cout << mean(v) << std::endl;
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
	// result of mean test: parallel mean is better twice 

	// variance test
	// /*
	std::cout << "variance:" << std::endl;
	for(int i = 10000; i < MAX_DIMENSION; i *= 10)
	{
		// generation of vector with vector.size = i
		std::vector<float> v(i);
		for(int j = 0 ; j < i; ++j)
		{
			v[j] = rand() / float(RAND_MAX);
		}

		omp_set_num_threads(1);
		time(&t);
		t_seq = clock(); // sequential variant start
		for(int j = 0; j < MAX_DIMENSION  / i; ++j)
		{
			variance(v);
		}
			std::cout << variance(v) << std::endl;
		t_seq = clock() - t_seq;
		float time1 = time(NULL) - t;

		omp_set_num_threads(2);
		time(&t);
		t_par = clock(); // parallel variant start
		for(int j = 0; j < MAX_DIMENSION / i; ++j)
		{
			variance(v);
		}
			std::cout << variance(v) << std::endl;

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
	// for 2 threads on my maschine
	// result of variance test: parallel mean is better nearly twice
	
	
	return 0;
}
