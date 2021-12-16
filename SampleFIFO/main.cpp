#include "Writer.h"
#include "Reader.h"
#include "SampleFIFO.h"

#include <iostream>
#include <strstream>
#include <fstream>




void oneFifoTransaction(SampleFIFO& fifo)
{
	const char* input = "hello world!";
	char output[128] = { 0 };

	size_t count = 3;
	void * data = fifo.getFree(count);
	size_t size = count * fifo.getBlockSize();
	if (data == nullptr)
	{
		std::cout << "fail to get free" << std::endl;
		return;
	}
	memcpy(data, input, size);
	fifo.addReady(2);

	size_t blocks = count;
	data = fifo.getReady(blocks);
	if (data == nullptr)
	{
		std::cout << "fail to get ready" << std::endl;
		return;
	}
	memcpy(output, data, blocks * fifo.getBlockSize());
	fifo.addFree(blocks);
	std::cout << output << std::endl;


}

int main()
{
	SampleFIFO fifo(4, 8);
	std::istrstream istr("Hello world bitches");
	std::ifstream ifstr("text.txt");
	if (!ifstr.is_open())
	{
		std::cout << "fool" << std::endl;
		return 1;
	}
	Writer wr(fifo, istr);
	Reader reader(fifo, std::cout);

	auto write = [&wr] {return wr.write(4); };
	auto read = [&reader] {return reader.read(4); };

	oneFifoTransaction(fifo);

	//std::thread thrW(write);
	//std::thread thrR(read);


	//thrW.join();
	//thrR.join();
	ifstr.close();
	return 0;
}