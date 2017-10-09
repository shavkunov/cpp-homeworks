#ifndef AUTOTEST_HPP
#define AUTOTEST_HPP

#include "huffman_test.h"
#include "base_test.h"
#include <vector>

class Autotest {
public:
	Autotest(size_t amount_of_tests);
	void run();

private:
	size_t success, tests;

};

#endif //AUTOTEST_HPP