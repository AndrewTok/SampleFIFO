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
	while (currPtr < input + inputSize)
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
		}


		size_t blocks = 1;
		data = fifo.getReady(blocks);
		if (data == nullptr)
		{
			std::cout << "fail to get ready" << std::endl;

		}
		else
		{
			memcpy(currOut, data, blocks * fifo.getBlockSize());
			fifo.addFree(data);
			currOut += blocks * fifo.getBlockSize();
			//output[blocks * fifo.getBlockSize()] = '\0';
			//std::cout << output;// << std::endl;
		}

	}
	std::cout << output << std::endl;

}

int main()
{
	SampleFIFO fifo(2, 16);
	std::istrstream istr("Hello world");
	std::ifstream ifstr("text.txt");
	std::strstream ostr;
	if (!ifstr.is_open())
	{
		std::cout << "cant open" << std::endl;
		return 1;
	}
	Writer wr(fifo, ifstr);
	Reader reader(fifo, ostr);

	auto write = [&wr] {return wr.write(4); };
	auto read = [&reader] {return reader.read(4); };


	const char* data = "Hello world Hello world Hello world Hello world Hello world";
	oneFifoTransaction(fifo, data, 100);

	//std::thread thrW(write);
	//std::thread thrR(read);

	////read();

	//thrW.join();
	//thrR.join();
	

	

	std::string out;
	//ostr >> out;

	while (!ostr.eof())
	{
		std::cout << char(ostr.get());
	}

	std::cout << std::endl << out << std::endl;
	ifstr.close();
	return 0;
}