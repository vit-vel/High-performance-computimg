template<class Container>
double mean(Container& cnt) {
    double sum = 0;
    size_t size = cnt.size();

	#pragma	omp parallel for shared(cnt) reduction(+:sum)  
    for(size_t i = 0; i < size; ++i)
    {
    	sum += (cnt[i] / size);
    }
    return sum;
}

template<class Container>
double variance(Container& cnt) {
    double m = mean(cnt);
    double sum = 0;
    size_t size = cnt.size();

    #pragma omp parallel for shared(cnt) reduction(+:sum)
    for(size_t i = 0; i < size; i+=1)
	{
		sum += (m - cnt[i]) * (m - cnt[i]) / size;
	}

	return sum;
}
