#include "Writer.h"
using namespace std::chrono;

void Writer::writeBlocks(void * destination, std::deque<char>& data, size_t& count)
{
	if (destination == nullptr)
	{
		return;
	}
	for (size_t index = 0; index < count * sampleFifo.getBlockSize(); ++index)
	{
		if (data.size() == 0)
		{
			count = (index-1)/sampleFifo.getBlockSize();
			break;
		}
		*((char*)destination + index) = data[0];
		data.pop_front();
	}
}

void Writer::prepareData(std::deque<char>& data, size_t bytesNum) //догрузить данные
{
	char currByte;
	while (bytesNum != 0 && !dataSource.eof())
	{
		dataSource.get(currByte);
		data.push_back(currByte);
		--bytesNum;
	}
}

void Writer::printData(std::deque<char>& data) const
{
	std::cout << "writer prints: ";
	for (auto& c : data)
	{
		std::cout << c;
	}
	std::cout << std::endl;
}


int Writer::write(size_t dataPortionSize)
{
	if (dataPortionSize > sampleFifo.getFullSize())
	{
		//throw std::out_of_range("too big data portion");
		return 1;
	}

	std::cout << std::endl << "writer: " << std::this_thread::get_id() << std::endl;
	void* dest;
	size_t blocksCount = dataPortionSize > sampleFifo.getBlockSize() ? dataPortionSize / sampleFifo.getBlockSize() : 1;
	std::deque<char> data;
	prepareData(data, dataPortionSize);
	size_t badLoopCount = 0;
	size_t maxBadLoopCount = 64;
	while (badLoopCount < maxBadLoopCount)
	{
		//printData(data);
		
		//std::cout << std::endl << "writer start loop" << std::endl;
		//sampleFifo.printStat();
		//std::this_thread::sleep_for(20s);
		blocksCount = data.size() / sampleFifo.getBlockSize(); //data.size() > sampleFifo.getBlockSize() ? data.size() / sampleFifo.getBlockSize() : 1;
		dest = sampleFifo.getFree(blocksCount);
		if (dest == nullptr)
		{
			++badLoopCount;
			std::this_thread::sleep_for(100ms); //wait for new data is available 
			//std::cout << std::endl << "writer end loop" << std::endl;
			continue;
		}
		else
		{
			writeBlocks(dest, data, blocksCount);
			sampleFifo.addReady(dest);
			prepareData(data, dataPortionSize);
			//std::cout << std::endl << "writer end loop" << std::endl;
			//printData(data);
		}
	}
	return 0;
}
