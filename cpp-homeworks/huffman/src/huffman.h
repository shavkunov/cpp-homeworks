#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include "constants.h"
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
#include <cstddef>
#include <functional>

class Huffman {
public:
	Huffman(); 

	void encode(std::string in_file_name, std::string out_file_name);
	size_t build_trie();
	void get_symbols_code(size_t root, std::string code);
	void get_statistics(std::ifstream& in);
	void decode(std::string in_file_name, std::string out_file_name);

private:
	class Node {
	public:
		Node() {
			symbol = -1;
			child[0] = child[1] = -1;
		}
		size_t child[2];
		size_t symbol;
	};

	std::vector<Node> trie;
	size_t cnt[ALPHABET];
	std::string symbols_code[ALPHABET];

};

#endif //HUFFMAN_HPP