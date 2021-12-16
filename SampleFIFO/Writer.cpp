#include "Writer.h"

void Writer::writeBlocks(void * destination, std::deque<char>& data, size_t count)
{
	if (destination == nullptr)
	{
		return;
	}
	for (size_t index = 0; index < data.size(); ++index)
	{
		if (index >= count * sampleFifo.getBlockSize())
		{
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
		dataSource >> currByte;
		data.push_back(currByte);
		--bytesNum;
	}
}

void Writer::printData(std::deque<char>& data) const
{
	std::cout << "writer prints" << std::endl;
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

	
	void* dest;
	size_t blocksCount = dataPortionSize / sampleFifo.getBlockSize();
	std::deque<char> data;
	prepareData(data, dataPortionSize);
	size_t badLoopCount = 0;
	while (!dataSource.eof() && badLoopCount < 256)
	{
		//printData(data);
		if (data.size() < sampleFifo.getBlockSize())
		{
			prepareData(data, sampleFifo.getBlockSize() - data.size());
		}
		blocksCount = data.size() / sampleFifo.getBlockSize();
		dest = sampleFifo.getFree(blocksCount);
		if (dest == nullptr)
		{
			++badLoopCount;
			continue;
		}
		else
		{
			writeBlocks(dest, data, blocksCount);
			sampleFifo.addReady(blocksCount);
			prepareData(data, dataPortionSize);
		}
	}
	return 0;
}
