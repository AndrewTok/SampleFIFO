#pragma once
#include "SampleFIFO.h"
#include <vector>
#include <deque>
#include <iostream>

class Writer final
{

	SampleFIFO& sampleFifo;

	std::istream& dataSource;

	void writeBlocks(void* destinfation, std::deque<char>& data, size_t count);

	void prepareData(std::deque<char>& data, size_t count);

	void printData(std::deque<char>& data) const;

public:

	Writer(SampleFIFO& fifo, std::istream& _dataSource) : sampleFifo(fifo), dataSource(_dataSource) {}

	int write(size_t dataPortionSize);

	
};

