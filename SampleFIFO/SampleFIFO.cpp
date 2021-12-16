#include "SampleFIFO.h"

size_t SampleFIFO::getReadySize() const //смежные блоки
{
	if (m_nFree > m_nReady)
	{
		return (m_nFree - m_nReady - 1);
	}
	else if (m_nFree < m_nReady)
	{
		return (getFullSize() - m_nReady);
	}
	else
	{
		throw std::exception("ptrs in the same pos");
	}
}

size_t SampleFIFO::getFreeSize() const
{
	if (m_nFree > m_nReady)
	{
		return getFullSize() - m_nFree;
	}
	else if (m_nFree < m_nReady)
	{
		return m_nReady - m_nFree - 1;
	}
	else
	{
		throw std::exception("ptrs in the same pos");
	}
	
}

size_t SampleFIFO::getFullSize() const
{
	return blockSize*maxBlocks;
}

size_t SampleFIFO::getBlockSize() const
{
	return blockSize;
}

SampleFIFO::SampleFIFO(size_t _blockSize, size_t _maxBlocks) : blockSize(_blockSize), maxBlocks(_maxBlocks)
{
	m_pData = (new char[blockSize * maxBlocks]);
	m_pEnd = (m_pData + blockSize * maxBlocks);
	m_nFree = 1;
	m_nReady = 0;
}

SampleFIFO::~SampleFIFO()
{
	delete(m_pData);
}

void* SampleFIFO::getFree(size_t count)
{
	//std::lock_guard<std::mutex> guard(m_FifoMutex);
	if (count * blockSize > getFreeSize())
	{
		return nullptr;
	}
	return &m_pData[m_nFree];
}

void SampleFIFO::addReady(size_t count) // Добавить поддержку записи по кольцу?
{
	std::lock_guard<std::mutex> guard(m_FifoMutex);
	m_nFree = (m_nFree + count*blockSize) % getFullSize();
}

void* SampleFIFO::getReady(size_t& count)
{
	//std::lock_guard<std::mutex> guard(m_FifoMutex);
	if (count * blockSize > getReadySize())
	{
		if (getReadySize() == 0)
		{
			return nullptr;
		}
		count = getReadySize();
	}
	return &m_pData[m_nReady + 1];
}

void SampleFIFO::addFree(size_t count)
{
	std::lock_guard<std::mutex> guard(m_FifoMutex);
	m_nReady = (m_nReady + count*blockSize) % getFullSize();

}
