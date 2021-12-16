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

	size_t getReadySize() const; // ���������� ���� ��������� ��� ���������� �� �������
	size_t getFreeSize() const; // ���������� ���� ��������� ��� ������ � �������
	

public:

	SampleFIFO(size_t blockSize, size_t maxBlocks);

	~SampleFIFO();
	// ��� ������ � �������
	void* getFree(size_t count);
	void addReady(size_t count);

	// ��� ����������� �� �������
	void* getReady(size_t& count);
	void addFree(size_t count);

	size_t getFullSize() const; // ������ ���� �������
	size_t getBlockSize() const;
};

