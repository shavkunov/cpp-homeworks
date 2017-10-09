all: main.o 
	g++ -o main main.o 
	./main
main.o: main.cpp 
	g++ -c -std=c++11 -Wall -O2 main.cpp 

clean: 
	rm *.o main
