#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <string>

class Exception {
public:

	Exception(std::string problem);
	std::string what();

private:

	std::string message;

};

#endif //EXCEPTION_HPP