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
	volatile size_t m_nReadySize; // ������
	volatile size_t m_nFullSize; // delete in release

	const size_t blockSize;
	const size_t maxBlocks;
	
	size_t requestedFreeCount;
	void* givenFreePtr;
	
	size_t requestedReadyCount;
	void* givenReadyPtr;

	//size_t requestedBlocksCount;

	void printBuff() const;

	//���������� ����� � �����

public:

	SampleFIFO(size_t blockSize, size_t maxBlocks);

	// ��� ������ � �������
	void* getFree(size_t& count);
	void addReady(void * data); // void * 

	// ��� ����������� �� �������
	void* getReady(size_t& count); // ��� �� ���� ������
	void addFree(void * data);

	size_t getFullSize() const; // ������ ���� �������
	size_t getBlockSize() const;
	size_t getReadySize() const; // ���������� ������� ���� ��������� ��� ���������� �� �������
	size_t getFreeSize() const; // ���������� ������� ���� ��������� ��� ������ � �������

	void printStat();
};

