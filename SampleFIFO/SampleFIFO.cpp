#include "SampleFIFO.h"

size_t SampleFIFO::getReadySize() const
{
	return m_nReadySize;
}

size_t SampleFIFO::getFreeSize() const
{
	return m_nFreeSize;
}

size_t SampleFIFO::getFullSize() const
{
	return blockSize*maxBlocks;
}

size_t SampleFIFO::getBlockSize() const
{
	return blockSize;
}

void SampleFIFO::printStat()
{
	std::lock_guard<std::mutex> guard(m_FifoMutex);
	std::cout << std::endl;
	std::cout << std::this_thread::get_id() << " " << std::endl;
	std::cout << std::this_thread::get_id() << " " << "free space: " << m_nFreeSize << std::endl;
	std::cout << std::this_thread::get_id() << " " << "ready space: " << m_nReadySize << std::endl;
	std::cout << std::this_thread::get_id() << " " << "nfree space: " << m_nFree << std::endl;
	std::cout << std::this_thread::get_id() << " " << "nready space: " << m_nReady << std::endl;
	std::cout << std::this_thread::get_id() << " " << "buffer is: ";
	printBuff();
	std::cout << std::endl;
}

void SampleFIFO::printBuff() const
{
	for (auto& c : m_pData)
	{
		std::cout << c;
	}
	std::cout << std::endl;
}

SampleFIFO::SampleFIFO(size_t _blockSize, size_t _maxBlocks) : blockSize(_blockSize), maxBlocks(_maxBlocks), m_pData(_blockSize * _maxBlocks)
{
	m_nFree = 0;
	m_nReady = 0;
	m_nFreeSize = getFullSize();
	m_nReadySize = 0;
	m_nFullSize = blockSize * maxBlocks;
}

void* SampleFIFO::getFree(size_t& count)
{
	std::lock_guard<std::mutex> guard(m_FifoMutex);
	if (count == 0)
	{
		givenFreePtr = nullptr;
		return givenFreePtr;
	}
	else if (count * blockSize > m_nFreeSize)
	{
		if (m_nFreeSize / blockSize == 0)
		{
			givenFreePtr = nullptr;
			return givenFreePtr;
		}
		else
		{
			count = m_nFreeSize / blockSize;
		}
	}
	givenFreePtr = (void*)&(m_pData[m_nFree]);
	requestedFreeCount = count;
	return givenFreePtr;
}

void SampleFIFO::addReady(void* data)
{
	std::lock_guard<std::mutex> guard(m_FifoMutex);
	if (data != givenFreePtr)
	{
		return;
	}
	size_t count = requestedFreeCount;
	std::cout << std::endl << "start write" << std::endl;
	if (m_nFree > m_nReady)
	{
		m_nReadySize += count * blockSize;
		m_nFreeSize -= count * blockSize;
	}
	else if (m_nFree < m_nReady)
	{
		m_nFreeSize -= count * blockSize;
	}
	else if (m_nFree == m_nReady)
	{
		m_nReadySize += count * blockSize;
		m_nFreeSize -= count * blockSize;
	}
	m_nFree = (m_nFree + count*blockSize);
	if (m_nFree == m_nFullSize)
	{
		m_nFree = 0;
		m_nFreeSize = m_nReady;
	}
	std::cout << std::endl << "end write" << std::endl;
}

void* SampleFIFO::getReady(size_t& count)
{
	std::lock_guard<std::mutex> guard(m_FifoMutex);
	if (count == 0)
	{
		givenReadyPtr = nullptr;
		return givenReadyPtr;
	}
	if (count * blockSize > m_nReadySize)
	{
		if (m_nReadySize == 0)
		{
			givenReadyPtr = nullptr;
			return givenReadyPtr;
		}
		count = m_nReadySize / blockSize;
	}
	givenReadyPtr = &m_pData[m_nReady];
	requestedReadyCount = count;
	return givenReadyPtr;
}

void SampleFIFO::addFree(void* data)
{
	//вынести функцию сравнения free ready

	std::lock_guard<std::mutex> guard(m_FifoMutex);
	if (data != givenReadyPtr)
	{
		return;
	}

	std::cout << std::endl << "start read" << std::endl;

	size_t count = requestedReadyCount;

	if (m_nReady > m_nFree)
	{
		m_nFreeSize+= count * blockSize;
		m_nReadySize -= count * blockSize;
	}
	else if (m_nReady < m_nFree)
	{
		m_nReadySize -= count * blockSize;
	}
	else if (m_nFree == m_nReady)
	{
		m_nReadySize -= count * blockSize;
		m_nFreeSize += count * blockSize;
	}
	m_nReady = (m_nReady + count * blockSize);
	if (m_nReady == m_nFullSize)
	{
		m_nReady = 0;
		m_nReadySize = m_nFree;
	}


	std::cout << "end read" << std::endl;
}
