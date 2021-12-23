#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include <vector>

class SampleFIFO final
{
	std::mutex m_FifoMutex;
	std::vector<char> m_pData; 

	std::atomic<size_t> m_nReady;
	std::atomic<size_t> m_nFree;
	std::atomic<size_t> m_nFreeSize;
	std::atomic<size_t> m_nReadySize;
	std::atomic<size_t> m_nFullSize;


	const size_t blockSize;
	const size_t maxBlocks;
	
	size_t requestedFreeCount;
	void* givenFreePtr;
	
	size_t requestedReadyCount;
	void* givenReadyPtr;

	void printBuff() const;

public:

	SampleFIFO(size_t blockSize, size_t maxBlocks);

	void* getFree(size_t& count);
	void addReady(void * data); 

	void* getReady(size_t& count);
	void addFree(void * data);

	size_t getFullSize() const; 
	size_t getBlockSize() const;
	size_t getReadySize() const; 
	size_t getFreeSize() const;

	
	void printStat();
};

