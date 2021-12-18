#include "Reader.h"

using namespace std::chrono;

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
	std::cout << "reader prints: ";
	for (auto& c : data)
	{
		std::cout << c;
	}
	std::cout << std::endl;
}

size_t Reader::getStartBlocksCount(size_t dataPortionSize) const
{
	return dataPortionSize > sampleFifo.getBlockSize() ? dataPortionSize / sampleFifo.getBlockSize() : 1;
}

int Reader::read(size_t dataPortionSize)
{

	//std::cout << std::endl << "reader: " << std::this_thread::get_id() << std::endl;
	
	size_t startBlocksCount = getStartBlocksCount(dataPortionSize);
	std::deque<char> data;
	
	bool reading = true;
	while (reading)
	{	
		size_t blocksCount = startBlocksCount;
		void* source = sampleFifo.getReady(blocksCount);
		if (source == nullptr)
		{
			if (sampleFifo.isDataTransfering())
			{
				std::this_thread::sleep_for(10ms); //wait for new data
			}
			else
			{
				reading = false;
			}
		}
		else
		{
			data = readData(source, blocksCount*sampleFifo.getBlockSize());
			storeData(data);
			sampleFifo.addFree(source);
		}
	}
	return 0;
}
