/* 
 * File:   utility.h
 * Author: phpwind
 *
 * Created on 2010年12月22日, 下午4:14
 */

#ifndef UTILITY_H
#define	UTILITY_H
#include <string>
#include <sstream>

template<typename T>
inline std::string to_string(const T & Value) {
    std::stringstream streamOut;
    streamOut << Value;
    return streamOut.str();
}

template<typename T>
inline T from_string(const std::string & ToConvert) {
    std::stringstream streamIn(ToConvert);
    T ReturnValue = T();
    streamIn >> ReturnValue;
    return ReturnValue;
}

template<typename TOUT, typename TIN>
TOUT convert(const TIN & in) {
    std::stringstream stream;
    stream << in;
    TOUT result;
    stream >> result;
    return result;
}

#endif	/* UTILITY_H */

