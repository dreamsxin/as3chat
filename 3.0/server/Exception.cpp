#include "Exception.h"

Exception::Exception(std::string message, int number, int code) throw () :
mMessage(message) {
}

Exception::~Exception() throw () {
}

const char * Exception::what() const throw () {
    return mMessage.c_str();
}
