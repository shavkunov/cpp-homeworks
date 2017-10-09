#include "huffman.h"
#include "exception.h"
#include "../test/autotest.h"

#include <ctime>
#include <cstring>
#include <iostream>

void print_usage() {
	std::cout << "Usage: ./huffman [-c | -u] -f input_file -o output_file" << '\n';
}

class Parser {
public:
	Parser() {};

	void parse(Huffman& h, int argc, char* argv[]) {
		bool zip = false, unzip = false;
		std::string in_file_name = "", out_file_name = "";

		if (argc == 2) {
			int tests = atoi(argv[1]);
			Autotest a(tests);
			a.run();
			return;
		}

		for (size_t i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-c") == 0) {
				zip = true;
			} 

			if (strcmp(argv[i], "-u") == 0) {
				unzip = true;
			}

			if (strcmp(argv[i], "-f") == 0) {
				if (i + 1 >= argc)
					throw Exception("problems with input_file");
				in_file_name = argv[i + 1];
				i++;
			}

			if (strcmp(argv[i], "-o") == 0) {
				if (i + 1 >= argc)
				 	throw Exception("problems with output_file");
				out_file_name = argv[i + 1];
				i++;
			}
		}


		if ((zip && unzip) || (!zip && !unzip) || (in_file_name == "" || out_file_name == "")) {
			 if ((zip && unzip) || (!zip && !unzip)) 
			 	throw Exception("problems with flags");

			 if ((in_file_name == "" || out_file_name == "")) 
			 	throw Exception("problems with files");
		}

		if (zip)
			h.encode(in_file_name, out_file_name);

		if (unzip)
			h.decode(in_file_name, out_file_name);
	}
};


int main(int argc, char* argv[]) {

	Huffman huffman;
	Parser parser;
	
	try {
		parser.parse(huffman, argc, argv);
	} 
	catch(Exception& e) {

		std::cout << e.what() << '\n';
		print_usage();
		exit(1);
	}

	return 0;
}