#ifndef BINARY_WRITER_HPP
#define BINARY_WRITER_HPP

#include "constants.h"
#include <string>
#include <fstream>
#include <cstddef>
#include <bitset>

class BinaryWriter {
public:

	BinaryWriter(std::ofstream& stream);
	~BinaryWriter();

	void add(char bit);
	void write(size_t info);
	void reset_buffer();

private:

	std::ofstream& ofs;
	unsigned char buf;
	size_t cur_bit;

};

#endif //BINARY_WRITER_HPP