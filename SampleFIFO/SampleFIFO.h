#pragma once
#include <thread>
#include <mutex>
#include <memory> // for shared ptr (may be)
#include <iostream>
#include <vector>

class SampleFIFO final
{
	std::mutex m_FifoMutex;
	std::vector<char> m_pData; //const u
	volatile size_t m_nReady;
	volatile size_t m_nFree;
	volatile size_t m_nFreeSize;
	volatile size_t m_nReadySize; // размер
	volatile size_t m_nFullSize; // delete in release

	const size_t blockSize;
	const size_t maxBlocks;
	
	size_t requestedFreeCount;
	void* givenFreePtr;
	
	size_t requestedReadyCount;
	void* givenReadyPtr;

	//size_t requestedBlocksCount;

	void printBuff() const;

	//запоминать адрес и длину

public:

	SampleFIFO(size_t blockSize, size_t maxBlocks);

	// для записи в очередь
	void* getFree(size_t& count);
	void addReady(void * data); // void * 

	// для считываения из очереди
	void* getReady(size_t& count); // тот ли блок отдали
	void addFree(void * data);

	size_t getFullSize() const; // размер всей очереди
	size_t getBlockSize() const;
	size_t getReadySize() const; // количетсво смежных байт доступных для считывания из очереди
	size_t getFreeSize() const; // количество смежных байт доступных для записи в очередь

	void printStat();
};

