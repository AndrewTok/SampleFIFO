#include "Writer.h"
#include "Reader.h"
#include "SampleFIFO.h"

#include <iostream>
#include <strstream>
#include <fstream>
#include <string>





void oneFifoTransaction(SampleFIFO& fifo, const char* input, size_t inputSize)
{
	char output[2048] = { 0 };
	char* currOut = output;
	const char* currPtr = input;
	size_t count = 3;
	bool writerFinish = false, readerFinish = false;
	while (!writerFinish || !readerFinish)
	{
		if (!writerFinish)
		{
			void* data = fifo.getFree(count);
			size_t size = count * fifo.getBlockSize();
			if (data == nullptr)
			{
				std::cout << "fail to get free" << std::endl;

			}
			else
			{
				memcpy(data, currPtr, size);
				fifo.addReady(data);
				currPtr += size;
				if (currPtr >= input + inputSize)
				{
					writerFinish = true;
				}
			}
		}

		size_t blocks = 1;
		void * data = fifo.getReady(blocks);
		if (data == nullptr)
		{
			if (!writerFinish)
			{
				std::cout << "fail to get ready" << std::endl;
			}
			else
			{
				readerFinish = true;
			}
		}
		else
		{
			memcpy(currOut, data, blocks * fifo.getBlockSize());
			fifo.addFree(data);
			currOut += blocks * fifo.getBlockSize();
		}
	}
	std::cout << output << std::endl;

}

void prtintStream(std::istream& str)
{
	str.clear();
	str.seekg(0, std::ios::beg);
	while (!str.eof())
	{
		std::cout << char(str.get());
	}
	std::cout << std::endl;
	str.clear();
	str.seekg(0, std::ios::beg);
}

int main()
{
	SampleFIFO fifo(1, 21);
	std::istringstream istr("Hello world");
	std::ifstream ifstr("text.txt");
	prtintStream(ifstr);
	std::stringstream ostr;
	if (!ifstr.is_open())
	{
		std::cout << "cant open" << std::endl;
		return 1;
	}
	Writer wr(fifo, istr);
	Reader reader(fifo, ostr);

	auto write = [&wr] {return wr.write(); };
	auto read = [&reader] {return reader.read(); };


	const char* data = "Hello world Hello world Hello world Hello world Hello world";
	oneFifoTransaction(fifo, data, 60);


	std::thread thrW(write);
	std::thread thrR(read);

	thrW.join();

	reader.finish();

	thrR.join();
	

	while (!ostr.eof())
	{
		std::cout << char(ostr.get());
	}


	ifstr.close();
	return 0;
}