#ifndef BINARY_READER_HPP
#define BINARY_READER_HPP

#include "constants.h"
#include <string>
#include <fstream>
#include <cstddef>

class BinaryReader {
public:

	BinaryReader(std::ifstream& stream);

	bool read_bit(char& bit);
	void read(size_t& info);
	void read_padding();
	void read_byte();

private:

	std::ifstream& ifs;
	char buf;
	bool success;
	size_t cur_bit, padding, file_length, cursor;
};

#endif //BINARY_READER_HPP