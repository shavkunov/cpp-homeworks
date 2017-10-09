FLAGS = -std=c++11 -O3

all : make_directory bin/huffman

make_directory:
	@mkdir -p bin

bin/huffman : bin/main.o bin/huffman.o bin/exception.o bin/BinaryReader.o bin/BinaryWriter.o bin/autotest.o bin/huffman_test.o
	g++ $(FLAGS) bin/main.o bin/huffman.o bin/exception.o bin/BinaryReader.o bin/BinaryWriter.o bin/autotest.o bin/huffman_test.o -o bin/huffman

bin/main.o : src/main.cpp src/huffman.h src/exception.h
	g++ -c $(FLAGS) src/main.cpp -o bin/main.o

bin/huffman.o : src/huffman.cpp src/huffman.h src/BinaryReader.h src/BinaryWriter.h
	g++ -c $(FLAGS) src/huffman.cpp -o bin/huffman.o

bin/exception.o : src/exception.cpp src/exception.h
	g++ -c $(FLAGS) src/exception.cpp -o bin/exception.o

bin/BinaryReader.o : src/BinaryReader.cpp src/BinaryReader.h src/constants.h
	g++ -c $(FLAGS) src/BinaryReader.cpp -o bin/BinaryReader.o

bin/BinaryWriter.o : src/BinaryWriter.cpp src/BinaryWriter.h src/constants.h
	g++ -c $(FLAGS) src/BinaryWriter.cpp -o bin/BinaryWriter.o

bin/autotest.o : test/autotest.cpp test/autotest.h test/huffman_test.h test/base_test.h 
	g++ -c $(FLAGS) test/autotest.cpp -o bin/autotest.o

bin/huffman_test.o : test/huffman_test.cpp test/base_test.h test/huffman_test.h src/constants.h src/huffman.h
	g++ -c $(FLAGS) test/huffman_test.cpp -o bin/huffman_test.o 

clean: 
	rm -r bin