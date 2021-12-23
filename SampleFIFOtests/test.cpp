#include "pch.h"
#include <vector>
#include <thread>
#include "SampleFIFO.h"
#include "Writer.h"
#include "Reader.h"
#include <iterator>
#include <fstream>
#include <strstream>

template<class InputIt>
void writeData(char* dest, InputIt first, InputIt last)
{
	if (dest == nullptr)
	{
		return;
	}
	size_t shift = 0;
	for (InputIt iter = first; iter != last; ++iter)
	{
		dest[shift] = (*iter);
		++shift;
	}
}

template<class InputIt>
void storeData(SampleFIFO& fifo, InputIt src, size_t& writeCount)
{
	void* dest = fifo.getFree(writeCount);
	ASSERT_NE(dest, nullptr);
	writeData((char*)(dest), src, src + writeCount * fifo.getBlockSize());
	fifo.addReady(dest);
}

template<class OutIt>
void loadData(SampleFIFO& fifo, OutIt dest, size_t& readCount)
{
	void* src = fifo.getReady(readCount);
	ASSERT_NE(src, nullptr);
	readData((char*)src, dest, dest + readCount * fifo.getBlockSize());
	fifo.addFree(src);
}

template<class OutIt>
void readData(char* source, OutIt first, OutIt last)
{
	if (source == nullptr)
	{
		return;
	}
	size_t pos = 0;
	for (OutIt i = first; i != last; ++i)
	{
		*i = source[pos];
		++pos;
	}
}

std::vector<char> getData(size_t size)
{
	std::vector<char> data;
	for (size_t i = 0; i < size; ++i)
	{
		data.push_back(char(i));
	}
	return data;
}

void fillStream(std::ostream& stream, size_t size)
{
	stream.clear();
	stream.seekp(0, std::ios::beg);
	for (size_t i = 0; i < size; ++i)
	{
		stream.put(char(i % 256 + 128));
	}
	stream.clear();
	stream.seekp(0, std::ios::beg);
}

void cmpStreams(std::istream& one, std::istream& another)
{
	one.clear();
	another.clear();
	one.seekg(0, std::ios::beg);
	another.seekg(0, std::ios::beg);
	while (!one.eof() && !another.eof())
	{
		char cOne = one.get();
		char cAnother = another.get();
		ASSERT_EQ(cOne, cAnother);
	}
	ASSERT_TRUE(one.eof() && another.eof());
}


void transferData(SampleFIFO& fifo, std::istream& input, std::ostream& output)
{
	Writer writer(fifo, input);
	Reader reader(fifo, output);

	std::thread writerThread([&writer]() { writer.write(); });
	std::thread readerThread([&reader]() { reader.read(); });

	writerThread.join();

	reader.finish();

	readerThread.join();
}

TEST(FifoOneThread, WriteReadSameSize)
{
	size_t blockSize = 2;
	size_t blocksNum = 6;
	SampleFIFO fifo(blockSize, blocksNum);
	size_t writeCount = 2;
	size_t readCount = 2;
	std::vector<char> data = getData(blockSize * 2);
	std::vector<char> rData(data.size());
	for (size_t transactionNum = 0; transactionNum < 42; ++transactionNum)
	{
		storeData(fifo, data.begin(), writeCount);

		loadData(fifo, rData.begin(), readCount);

		ASSERT_EQ(writeCount, readCount);
		ASSERT_EQ(rData, data);
	}
}

TEST(FifoOneThread, WriteReadFullBuffer)
{
	size_t blockSize = 3;
	size_t blocksNum = 2;
	SampleFIFO fifo(blockSize, blocksNum);
	size_t writeCount = 2;
	size_t readCount = 2;
	std::vector<char> data = { '1', '2', '3', '4', '5', '6' };
	std::vector<char> rData(data.size());
	for (size_t transactionNum = 0; transactionNum < 42; ++transactionNum)
	{
		storeData(fifo, data.begin(), writeCount);

		ASSERT_EQ(writeCount, 2);

		loadData(fifo, rData.begin(), readCount);

		ASSERT_EQ(writeCount, readCount);
		ASSERT_EQ(rData, data);
	}
}


TEST(FifoOneThread, TryWriteInFullBuff)
{
	size_t blockSize = 3;
	size_t blocksNum = 2;
	SampleFIFO fifo(blockSize, blocksNum);
	size_t writeCount = 2;
	size_t readCount = 2;
	std::vector<char> data = { '1', '2', '3', '4', '5', '6' };
	void* dest = fifo.getFree(writeCount);

	storeData(fifo, data.begin(), writeCount);

	ASSERT_EQ(writeCount, 2);

	dest = fifo.getFree(writeCount);
	ASSERT_EQ(dest, nullptr);

}

TEST(FifoOneThread, ZeroBlockSizeTest)
{
	size_t blockSize = 0;
	size_t blocksNum = 2;
	SampleFIFO fifo(blockSize, blocksNum);
	size_t writeCount = 2;
	std::vector<char> data = { '1', '2', '3', '4', '5', '6' };
	void* dest = fifo.getFree(writeCount);
	ASSERT_EQ(dest, nullptr);
	dest = fifo.getReady(writeCount);
	ASSERT_EQ(dest, nullptr);
}

TEST(FifoOneThread, ZeroBlockNumTest)
{
	size_t blockSize = 2;
	size_t blocksNum = 0;
	SampleFIFO fifo(blockSize, blocksNum);
	size_t writeCount = 2;
	std::vector<char> data = { '1', '2', '3', '4', '5', '6' };
	void* dest = fifo.getFree(writeCount);
	ASSERT_EQ(dest, nullptr);
	dest = fifo.getReady(writeCount);
	ASSERT_EQ(dest, nullptr);
}

TEST(FifoOneThread, DataGreaterThenBuffTest)
{
	size_t blockSize = 5;
	size_t blocksNum = 7;
	SampleFIFO fifo(blockSize, blocksNum);
	size_t writeCount = 2;
	size_t readCount = 2;
	std::vector<char> data = getData(blockSize*1024);
	std::vector<char> rData(data.size());
	auto readIter = rData.begin();
	for (auto writeIter = data.begin(); writeIter != data.end();)
	{
		writeCount = std::distance(writeIter, data.end()) / blockSize;
		storeData(fifo, writeIter, writeCount);

		readCount = writeCount;
		loadData(fifo, readIter, readCount);

		readIter += readCount * blockSize;
		writeIter += writeCount * blockSize;

	}
	ASSERT_EQ(data, rData);
}

TEST(FifoMultithreading, BigDataTransfer)
{
	size_t blockSize = 3;
	size_t blocksNum = 256;

	SampleFIFO fifo(blockSize, blocksNum);

	std::fstream input("input.txt");
	std::fstream output("output.txt");
	
	ASSERT_TRUE(input.is_open() && output.is_open());

	transferData(fifo, input, output);

	cmpStreams(input, output);

	input.close();
	output.close();
}

TEST(FifoMultithreading, ZeroSizeData)
{
	size_t blockSize = 3;
	size_t blocksNum = 113;

	SampleFIFO fifo(blockSize, blocksNum);

	std::stringstream input;
	std::stringstream output;
	std::stringstream empty;


	transferData(fifo, input, output);

	cmpStreams(input, empty);
	cmpStreams(output, empty);
}

TEST(FifoMultithreading, SmallBlockSizeSmallBlocksNum)
{
	for (size_t blocksNum = 1; blocksNum < 11; ++blocksNum)
	{
		std::cout << double(blocksNum) / 11.0 * 100 << "%" << std::endl;
		for (size_t blockSize = 1; blockSize < 23; ++blockSize)
		{
			SampleFIFO fifo(blockSize, blocksNum);

			std::stringstream input;
			std::stringstream output;

			fillStream(input, blockSize * 5);

			transferData(fifo, input, output);

			cmpStreams(input, output);
		}
	}
}

TEST(FifoMultithreading, SmallBlockSizeBigBlocksNum)
{
	for (size_t blocksNum = 4086; blocksNum < 4100; ++blocksNum)
	{
		std::cout << double(blocksNum) / 4100.0 * 100 << "%" << std::endl;
		for (size_t blockSize = 1; blockSize < 23; ++blockSize)
		{
			SampleFIFO fifo(blockSize, blocksNum);

			std::stringstream input;
			std::stringstream output;

			fillStream(input, blockSize * 5);

			transferData(fifo, input, output);

			cmpStreams(input, output);
		}
	}
}

TEST(FifoMultithreading, BigBlockSizeSmallBlocksNum)
{
	for (size_t blocksNum = 1; blocksNum < 11; ++blocksNum)
	{
		std::cout << double(blocksNum) / 11.0 * 100 << "%" << std::endl;
		for (size_t blockSize = 4086; blockSize < 4100; ++blockSize)
		{
			SampleFIFO fifo(blockSize, blocksNum);

			std::stringstream input;
			std::stringstream output;

			fillStream(input, blockSize * 5);

			transferData(fifo, input, output);

			cmpStreams(input, output);
		}
	}
}


TEST(FifoMultithreading, BigBlockSizeBigBlocksNum)
{
	for (size_t blocksNum = 4086; blocksNum < 4100; ++blocksNum)
	{
		std::cout << double(blocksNum) / 4100.0 * 100 << "%" << std::endl;
		for (size_t blockSize = 4086; blockSize < 4100; ++blockSize)
		{
			SampleFIFO fifo(blockSize, blocksNum);

			std::stringstream input;
			std::stringstream output;

			fillStream(input, blockSize * 5);

			transferData(fifo, input, output);

			cmpStreams(input, output);
		}
	}
}