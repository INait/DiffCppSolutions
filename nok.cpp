#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <map>
#include <cmath>

#define N 10

std::vector<std::thread> threads;
std::condition_variable cv_;
std::mutex mutex_;

// 1 writer - N readers queue
std::queue< uint64_t > numbers;

std::atomic< bool > done_read{ false };

typedef std::map< uint64_t, uint64_t > PrimeMap;

// map prime factors
PrimeMap primeMap[ N ];
PrimeMap finalMap;

PrimeMap GeneratePrimeMap( uint64_t number )
{
	PrimeMap t_map;

	while( number > 1 )
	{
		for( uint64_t i = 2; i <= number; ++i )
		{
			if( number % i == 0 )
			{
				number /= i;
				t_map[i]++;
				break;
			}
		}
	}

	return t_map;
}

void ProcessNumbers(int threadNum)
{
	auto & thisMap = primeMap[ threadNum ];

	while( true )
	{
		std::unique_lock< std::mutex > lock( mutex_ );

		while( numbers.empty() && !done_read )
			cv_.wait( lock );

		if ( numbers.empty() && done_read )
			break;

		uint64_t number = numbers.front();
		numbers.pop();

		lock.unlock();

		// generate prime map and compare it with previous map, find local LCM
		auto t_map = GeneratePrimeMap( number );

		if( thisMap.empty() )
			thisMap = std::move( t_map );
		else
		{
			for( auto it : t_map )
				thisMap[ it.first ] = std::max( thisMap[ it.first ], it.second );

			for( auto it : thisMap )
				thisMap[ it.first ] = std::max( t_map[ it.first ], it.second );
		}
	}
}

int main(int argc, char* argv[])
{
	// initialize thread pool
	for( int i = 0; i < N; ++i )
	{
		std::thread thr( ProcessNumbers, i );
		threads.push_back( std::move( thr ) );
	}

	uint64_t cur_num;
	// Produce numbers
	for( int i = 1; i < argc; ++i )
	{
		cur_num = std::atoll( argv[i] );
		if( cur_num )
		{
			std::unique_lock< std::mutex > lock( mutex_ );
			numbers.push( cur_num );
			cv_.notify_one();
		}
	}

	while( std::cin >>  cur_num )
	{
		std::unique_lock< std::mutex > lock( mutex_ );
		numbers.push( cur_num );
		cv_.notify_one();
	}

	// notify about numbers ending
	std::unique_lock< std::mutex > lock( mutex_ );
	done_read = true;
	cv_.notify_all();
	lock.unlock();

	// wait for all threads to finish
	for( auto& th : threads )
		th.join();

	// generate final prime map from locals
	for( int i = 0; i < N; ++i )
	{
		if( primeMap[i].empty() )
			continue;

		if( finalMap.empty() )
			finalMap = primeMap[i];
		else
		{
			for( auto it : primeMap[i] )
				finalMap[ it.first ] = std::max( finalMap[ it.first ], it.second );

			for( auto it : finalMap )
				finalMap[ it.first ] = std::max( primeMap[ i ][ it.first ], it.second );
		}
	}

	for( PrimeMap::iterator it = finalMap.begin(); it != finalMap.end(); )
	{
		if( it->second == 0 )
			it = finalMap.erase(it);
		else
		{
			std::cout << it->first << "^" << it->second << " ";
			++it;
		}
	}

	// no common divisers
	if( finalMap.empty() )
		std::cout << "1";

	std::cout << std::endl;
	return 0;
}
