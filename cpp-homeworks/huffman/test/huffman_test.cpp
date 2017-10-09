#include "huffman_test.h"
#include "../src/constants.h"

#include <cstdlib>
#include <fstream>
#include <ctime>

void Huffman_Test::generate_test() {
	srand(std::time(NULL));
	size_t amount_of_symbols = std::rand() % MAX_TEST_RANGE;

	std::fstream f;
	f.open(INPUT_FILE, std::fstream::out | std::fstream::trunc);

	for (size_t i = 0; i < amount_of_symbols; i++) {
		char c = std::rand() % MAX_CHAR_RANGE;
		f.put(c);
	}
}

Huffman_Test::Huffman_Test () {}

bool Huffman_Test::check() {
	std::ifstream in(INPUT_FILE, std::ios::binary);
	std::ifstream res(DECODED_FILE, std::ios::binary);

	char char_in = 0, char_res = 0;
	while (in.get(char_in) && res.get(char_res)) {
		if (char_in != char_res)
			return false;
	}

	return true;
}

bool Huffman_Test::exec() {
	generate_test();

	Huffman huffman;
	huffman.encode(INPUT_FILE, ENCODED_FILE);
	huffman.decode(ENCODED_FILE, DECODED_FILE);

	return check();
}
