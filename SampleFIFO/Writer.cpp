#include "Writer.h"

using namespace std::chrono;

void Writer::writeBlocks(void * destination, std::deque<char>& data, size_t& count)
{
	if (destination == nullptr || data.empty())
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
		((char*)destination)[index] = data[0];
		data.pop_front();
	}
}

void Writer::prepareData(std::deque<char>& data, size_t bytesNum)
{
	while (bytesNum != 0 && !dataSource.eof())
	{
		char currByte = dataSource.get();
		if (dataSource.fail())
		{
			break;
		}
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

size_t Writer::getStartBlocksCount(size_t dataPortionSize) const
{
	return dataPortionSize > sampleFifo.getBlockSize() ? dataPortionSize / sampleFifo.getBlockSize() : 1;
}

bool Writer::thereIsDataToSend(std::deque<char>& data) const
{
	return !dataSource.eof() || ((data.size()) / sampleFifo.getBlockSize() > 0);
}


void Writer::write()
{
	size_t dataPortionSize = sampleFifo.getFreeSize();

	std::deque<char> data;
	while (thereIsDataToSend(data))
	{
		prepareData(data, dataPortionSize);
		size_t blocksCount = data.size() / sampleFifo.getBlockSize();
		void* destination = sampleFifo.getFree(blocksCount);
		if (destination == nullptr)
		{
			std::this_thread::sleep_for(1ms); //wait for new place is available 
			continue;
		}
		else
		{
			writeBlocks(destination, data, blocksCount);
			sampleFifo.addReady(destination);
		}
	}
}
