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
		throw std::out_of_range("cant write anymore");
	}
	m_nFree = (m_nFree + count*blockSize);
	if (m_nFree == m_nFullSize)
	{
		m_nFree = 0;
		m_nFreeSize = m_nReady;
	}


	//if (m_nFree == m_nReady)
	//{
	//	//free догнал ready и свободного места больше нет
	//	// размер ready не меняется
	//	m_nFreeSize = 0;

	//}
	//else if (m_nFree == m_nFullSize)
	//{
	//	// free дошел до края, перекинулся, размер ready - дистанция до края, размер free - дистанция до ready
	//	m_nFree = 0;
	//	m_nFreeSize = m_nReady;
	//	m_nReadySize += count * blockSize;
	//	if (m_nReadySize != m_nFullSize - m_nReady || m_nReady == m_nFullSize)
	//	{
	//		throw std::out_of_range("bad ready!!!!!"); // убрать после отладки
	//	}
	//}
	//else if (m_nFree > m_nFullSize)
	//{
	//	//free перешел через край, так не должно быть
	//	throw std::out_of_range("nfree out of border!!!!!"); // убрать после отладки
	//}
	//else if (m_nFree < m_nReady)
	//{
	//	//ready Не меняется, free уменьшается
	//	m_nFreeSize -= count * blockSize;
	//	if (m_nFreeSize != m_nReady - m_nFree)
	//	{
	//		throw std::out_of_range("bad nfree!!!!!"); // убрать после отладки
	//	}
	//}
	//else if (m_nFree > m_nReady)
	//{
	//	// размер ready увеличивается на сдвиг free и равен дистанции между ними, размер free уменьшается на сдвиг и равен дистанции до края
	//	m_nFreeSize -= count * blockSize;
	//	if (m_nFreeSize != m_nFullSize - m_nFree)
	//	{
	//		throw std::out_of_range("bad nfree!!!!!"); // убрать после отладки
	//	}
	//	m_nReadySize += count * blockSize;
	//	if (m_nReadySize != m_nFree - m_nReady)
	//	{
	//		throw std::out_of_range("bad ready!!!!!"); // убрать после отладки
	//	}
	//}
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
		throw std::out_of_range("can't read anymore");
	}
	m_nReady = (m_nReady + count * blockSize);
	if (m_nReady == m_nFullSize)
	{
		m_nReady = 0;
		m_nReadySize = m_nFree;
	}

	//m_nReady = (m_nReady + count*blockSize);

	//if (m_nReady == m_nFree)
	//{
	//	//ready догнал free b готового места больше нет
	//	// размер free не меняется
	//	m_nReadySize = 0;

	//}
	//else if (m_nReady == m_nFullSize)
	//{
	//	// ready дошел до края, перекинулся, размер free - дистанция до края, размер ready - дистанция до free
	//	m_nReady = 0;
	//	m_nReadySize = m_nFree;
	//	m_nFreeSize += count * blockSize;
	//	if (m_nFreeSize != m_nFullSize - m_nFree || m_nFree == m_nFullSize)
	//	{
	//		throw std::out_of_range("bad free!!!!!"); // убрать после отладки
	//	}
	//}
	//else if (m_nReady > m_nFullSize)
	//{
	//	//ready перешел через край, так не должно быть
	//	throw std::out_of_range("nready out of border!!!!!"); // убрать после отладки
	//}
	//else if (m_nReady < m_nFree)
	//{
	//	//free Не меняется, ready уменьшается
	//	m_nReadySize -= count * blockSize;
	//	if (m_nReadySize != m_nFree - m_nReady)
	//	{
	//		throw std::out_of_range("bad nready!!!!!"); // убрать после отладки
	//	}
	//}
	//else if (m_nReady > m_nFree )
	//{
	//	// размер free увеличивается на сдвиг ready и равен дистанции между ними, размер ready уменьшается на сдвиг и равен дистанции до края
	//	m_nReadySize -= count * blockSize;
	//	if (m_nReadySize != m_nFullSize - m_nReady)
	//	{
	//		throw std::out_of_range("bad nready!!!!!"); // убрать после отладки
	//	}
	//	m_nFreeSize += count * blockSize;
	//	if (m_nFreeSize != m_nReady - m_nFree)
	//	{
	//		throw std::out_of_range("bad free!!!!!"); // убрать после отладки
	//	}
	//}

	std::cout << "end read" << std::endl;
}
