/* 
 * File:   Exception.h
 * Author: myleft
 *
 * Created on 2010年12月17日, 上午12:23
 */

#ifndef EXCEPTION_H
#define	EXCEPTION_H
#include <iostream>
#include <string>
using namespace std;

class Exception {
public:
    Exception(string message, int number = -1, int code = -1) throw ();
    virtual ~Exception() throw ();
    virtual const char * what() const throw ();
private:
    string _message;
};
#endif	/* EXCEPTION_H */

