#pragma once
#include <thread>
#include <mutex>
#include <memory> // for shared ptr (may be)


class SampleFIFO final
{
	std::mutex m_FifoMutex;
	char* m_pData; //const
	size_t m_nReady;
	size_t m_nFree;
	char* m_pEnd; //const

	size_t blockSize;
	size_t maxBlocks;
	
	//size_t requestedBlocksCount;

	size_t getReadySize() const; // количетсво байт доступных для считывания из очереди
	size_t getFreeSize() const; // количество байт доступных для записи в очередь
	

public:

	SampleFIFO(size_t blockSize, size_t maxBlocks);

	~SampleFIFO();
	// для записи в очередь
	void* getFree(size_t count);
	void addReady(size_t count);

	// для считываения из очереди
	void* getReady(size_t& count);
	void addFree(size_t count);

	size_t getFullSize() const; // размер всей очереди
	size_t getBlockSize() const;
};

