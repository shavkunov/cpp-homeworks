#include "BinaryWriter.h"

#include <iostream>
#include <bitset>

BinaryWriter::BinaryWriter(std::ofstream& stream) : ofs(stream), buf(0), cur_bit(0) {}

void BinaryWriter::reset_buffer() {
	cur_bit = 0;
	ofs.put(buf);
	buf = 0;
}

void BinaryWriter::write(size_t info) {
	unsigned char buffer = 0;
	for (size_t i = 0; i < 4; i++) {
		buffer = info & (255);
		ofs.put(buffer);
		info >>= BITS_IN_BYTE;
	}
}

void BinaryWriter::add(char bit) {
	
	if (bit == '1') {
		buf |= 1 << cur_bit;
	}

	cur_bit++;

	if (cur_bit == BITS_IN_BYTE) {
		reset_buffer();
	}
}

BinaryWriter::~BinaryWriter() {
	ofs.put(buf);
}
