#pragma once
#include "SampleFIFO.h"
#include <vector>
#include <deque>
#include <iostream>

class Reader final
{
	SampleFIFO& sampleFifo;

	std::ostream& dataOutput;

	std::deque<char> readData(void* source, size_t count);

	void storeData(std::deque<char>& data);

	void printData(std::deque<char>& data);

public:

	Reader(SampleFIFO& fifo, std::ostream& out) : sampleFifo(fifo), dataOutput(out) {}

	int read(size_t dataPortionSize);
};

