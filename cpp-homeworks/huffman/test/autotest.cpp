#include "autotest.h"

#include <ctime>
#include <iostream>

Autotest::Autotest(size_t amount_of_tests) : tests(amount_of_tests), success(0) {}

void Autotest::run() {
	clock_t time;
	for (size_t i = 0; i < tests; i++) {
		time = clock();
		Huffman_Test ht;
		bool res = ht.exec();
		if (res)
			success++;

		time = clock() - time;

		std::cout.precision(6);
		std::cout << "Test№" << i + 1 << '\n';
		std::cout << std::fixed << "Run time : " << (double)time / CLOCKS_PER_SEC << " sec.\n";
	}

	std::cout << "Successful tests : " << success << '\n';
	std::cout << "Bad tests : " << tests - success << '\n'; 
}