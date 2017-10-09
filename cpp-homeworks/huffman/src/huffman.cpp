#include "huffman.h"
#include "BinaryWriter.h"
#include "BinaryReader.h"
#include <ctime>

void Huffman::get_statistics(std::ifstream& in) {
	char buffer = 0;

	in.seekg(0, in.beg);
	while (in.get(buffer)) {
		cnt[(unsigned char)buffer]++;
	}

	in.clear();
}

size_t Huffman::build_trie() {
	auto cmp = [] (const std::pair<size_t, size_t> a, const std::pair<size_t, size_t> b) {return a > b;};
	std::priority_queue<std::pair<size_t, size_t>, 
    			   std::vector<std::pair<size_t, size_t>>,
    			   decltype(cmp) > stats(cmp);


	size_t sz = 0;

	for (size_t cur_symb = 0; cur_symb < ALPHABET; cur_symb++) {
		if (cnt[cur_symb] > 0) {
			trie[sz].symbol = cur_symb;
			stats.push(std::make_pair(cnt[cur_symb], sz++));
		}
	}

	while (stats.size() > 1) {
		std::pair <size_t, size_t> min_symb1 = stats.top();
		stats.pop();
		std::pair <size_t, size_t> min_symb2 = stats.top();
		stats.pop();

		std::pair <size_t, size_t> sum;
        sum.first = min_symb1.first + min_symb2.first;
        sum.second = sz++;

        trie[sum.second].child[0] = min_symb1.second;
        trie[sum.second].child[1] = min_symb2.second;

        stats.push(sum);
	}

	return sz - 1;
}

void Huffman::get_symbols_code(size_t root, std::string code) {

	size_t left = trie[root].child[0];
    size_t right = trie[root].child[1];

    if (left != -1) {
        get_symbols_code(left, code + '0');
    }

    if (right != -1) {
        get_symbols_code(right, code + '1');
    }

    if (left == -1 && right == -1){
        size_t index = trie[root].symbol;
        symbols_code[index] = code;
    }

}

void Huffman::encode(std::string in_file_name, std::string out_file_name)  {
	size_t in_file_size, out_file_size, meta_info;

	std::ifstream in(in_file_name, std::ios::binary);

	get_statistics(in);

	size_t root = build_trie();

	if (root == -1) {
		std::ofstream out(out_file_name, std::ios::binary);
		std::cout << 0 << '\n' << 0 << '\n' << 0 << '\n';
		return;
	}

	get_symbols_code(root, "");

	std::ofstream out(out_file_name, std::ios::binary);
	BinaryWriter writer(out);
	for (size_t symb = 0; symb < ALPHABET; symb++) { 
		writer.write(cnt[symb]);
	}

	char buffer = 0;
    in.seekg (0, in.beg);

	long long text_len = 0;
	for (size_t i = 0; i < ALPHABET; i++) {
		text_len += cnt[i]*symbols_code[i].length();
	}

	size_t padding = BITS_IN_BYTE - text_len % BITS_IN_BYTE;
	writer.write(padding);
	meta_info = out.tellp();

	while (in.get(buffer)) {
		for (size_t i = 0; i < symbols_code[(unsigned char)buffer].length(); i++) {
			writer.add(symbols_code[(unsigned char)buffer][i]);
		}
		
	}

	out_file_size = out.tellp();
	in.clear();
    in_file_size = in.tellg();


	std::cout << in_file_size << '\n' << out_file_size << '\n' << meta_info << '\n';
}

void Huffman::decode(std::string in_file_name, std::string out_file_name) {
	size_t in_file_size, out_file_size, meta_info;

	std::ifstream in(in_file_name, std::ios::binary);
	BinaryReader reader(in);
	for (size_t symb = 0; symb < ALPHABET; symb++) {
		reader.read(cnt[symb]);
	}

	reader.read_padding();

	meta_info = in.tellg();
	size_t root = build_trie();

	if (root == -1) {
		std::ofstream out(out_file_name, std::ios::binary);
		std::cout << 0 << '\n' << 0 << '\n' << 0 << '\n';
		return;
	}

	char buffer = 0;
	std::ofstream out(out_file_name, std::ios::binary);
	
	while (reader.read_bit(buffer)) {
		size_t v = root;
		size_t to = buffer - '0';
		while(true) {
			v = trie[v].child[to];

			if (trie[v].child[to] == -1)
				break;

			reader.read_bit(buffer);
			to = buffer - '0';
		}

		out.put(trie[v].symbol);

	}

	in_file_size = in.tellg();
	out_file_size = out.tellp();

	std::cout << in_file_size << '\n' << out_file_size << '\n' << meta_info << '\n';
}

Huffman::Huffman() {
	trie.resize(2*ALPHABET);
	std::fill(cnt, cnt + ALPHABET, 0);
}
