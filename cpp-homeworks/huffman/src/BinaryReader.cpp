#include "BinaryReader.h"

#include <iostream>
#include <bitset>

BinaryReader::BinaryReader(std::ifstream& stream) : ifs(stream), buf(0), cur_bit(BITS_IN_BYTE), cursor(0) {
	ifs.seekg (0, ifs.end);
    file_length = ifs.tellg();
    ifs.seekg (0, ifs.beg);
    success = 1;
    ifs.clear();
}

void BinaryReader::read_byte() {
	ifs.get(buf);
	cur_bit = 0;
	cursor++;
}

void BinaryReader::read(size_t& info) {
	char buffer = 0;
	info = 0;
	cursor += 4;
	for (size_t i = 0; i < SIZE_OF_BYTE; i++) {
		ifs.get(buffer);
		info |= (unsigned char)buffer << (BITS_IN_BYTE*i);
	}
}

void BinaryReader::read_padding() {
	read(padding);
}

bool BinaryReader::read_bit(char& bit) {
	if (cursor == file_length && (cur_bit + padding == BITS_IN_BYTE)) {
		success = 0;
		return success;
	}

	if (cur_bit == BITS_IN_BYTE) {
		read_byte();
	}

	if (cursor == file_length && (cur_bit + padding == BITS_IN_BYTE)) {
		success = 0;
		return success;
	}

	if ((buf & (1 << cur_bit)) == 0) {
		bit = '0';
	} else {
		bit = '1';
	}

	cur_bit++;
	return success;
}