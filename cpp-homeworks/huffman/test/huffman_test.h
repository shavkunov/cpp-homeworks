#ifndef HUFFMAN_TEST_HPP
#define HUFFMAN_TEST_HPP

#include "../src/huffman.h"
#include "base_test.h"

class Huffman_Test : public Test {
public:
	Huffman_Test();
	bool exec();
	void generate_test();
	bool check();
};

#endif //HUFFMAN_TEST_HPP