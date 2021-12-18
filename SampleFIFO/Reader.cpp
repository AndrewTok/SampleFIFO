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
		dataOutput << byte; //.put(byte); // 
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

int Reader::read(size_t dataPortionSize)
{

	std::cout << std::endl << "reader: " << std::this_thread::get_id() << std::endl;
	void* source;
	size_t startBlocksCount = dataPortionSize > sampleFifo.getBlockSize() ? dataPortionSize / sampleFifo.getBlockSize() : 1;
	std::deque<char> data;
	size_t maxbadLoopCount = 64;
	size_t badLoopCount = 0;
	while (badLoopCount < maxbadLoopCount)
	{	
		//blocksCount = dataPortionSize / sampleFifo.getBlockSize();
		//std::cout << std::endl << "reader start loop" << std::endl;
		sampleFifo.printStat();
		//std::this_thread::sleep_for(20s);
		size_t blocksCount = startBlocksCount;
		source = sampleFifo.getReady(blocksCount);
		if (source == nullptr)
		{
			badLoopCount++;
			std::this_thread::sleep_for(100ms); //wait for new data
			//std::cout << std::endl << "reader end loop" << std::endl;
			continue;
		}
		else
		{
			data = readData(source, blocksCount*sampleFifo.getBlockSize());
			storeData(data);
			sampleFifo.addFree(source);
			//std::cout << std::endl << "reader end loop" << std::endl;
			//printData(data);
		}
	}
	return 0;
}
