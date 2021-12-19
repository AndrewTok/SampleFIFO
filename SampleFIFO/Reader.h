#pragma once
#include "SampleFIFO.h"
#include <vector>
#include <deque>
#include <iostream>
#include <chrono>
#include <thread>

class Reader final
{
	SampleFIFO& sampleFifo;

	std::ostream& dataOutput;

	std::deque<char> readData(void* source, size_t count);

	void storeData(std::deque<char>& data);

	void printData(std::deque<char>& data);

	size_t getStartBlocksCount(size_t dataPortionSize) const;

public:

	Reader(SampleFIFO& fifo, std::ostream& out) : sampleFifo(fifo), dataOutput(out) {}

	void read();
};

