#include "exception.h"

Exception::Exception(std::string problem) : message(problem) {}

std::string Exception::what() {
	return message;
}