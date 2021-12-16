#include "Reader.h"

std::deque<char> Reader::readData(void* source, size_t bytesNum)
{
	if (source == nullptr)
	{
		return std::deque<char>();
	}
	std::deque<char> data;
	for (size_t index = 0; index < bytesNum; ++index)
	{
		data.push_back(*((char*)source + index));
	}
	return data;
}

void Reader::storeData(std::deque<char>& data)
{
	for (auto& byte : data)
	{
		dataOutput << byte;
	}
}

void Reader::printData(std::deque<char>& data)
{
	std::cout << "reader print" << std::endl;
	for (auto& c : data)
	{
		std::cout << c;
	}
	std::cout << std::endl;
}

int Reader::read(size_t dataPortionSize)
{
	if (dataPortionSize > sampleFifo.getFullSize())
	{
		//throw std::out_of_range("too big data portion");
		return 1;
	}


	void* source;
	size_t blocksCount = dataPortionSize > sampleFifo.getBlockSize() ? dataPortionSize / sampleFifo.getBlockSize() : 1;
	std::deque<char> data;
	size_t maxLoopCount = 256;
	size_t loopCount = 0;
	while (loopCount < maxLoopCount)
	{
		//printData(data);
		//blocksCount = dataPortionSize / sampleFifo.getBlockSize();
		source = sampleFifo.getReady(blocksCount);
		if (source == nullptr)
		{
			
			continue;
		}
		else
		{
			data = readData(source, blocksCount);
			storeData(data);
			sampleFifo.addFree(blocksCount);
		}
		loopCount++;
	}
	return 0;
}
