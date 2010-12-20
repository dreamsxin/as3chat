/*
 * File:   FifoList.h
 * Author: myleft
 *
 * Created on 2010年12月19日, 下午9:41
 */

#ifndef PACKERTBUFFER_H
#define	PACKERTBUFFER_H

#include <queue>
#include <stdexcept>
using std::queue;

template <typename T>
class FifoList {
public:
    FifoList(int maxSize = -1);
    void push(const T&packer);
    T pop() throw (std::out_of_range);
protected:
    queue<T> mPackets;
    int mMaxSize;
private:
    FifoList(const FifoList& src);
    FifoList & operator=(const FifoList& rhs);
};

template <typename T>
FifoList<T>::FifoList(int maxSize) {
    mMaxSize = maxSize;
}

template <typename T>
void FifoList<T>::push(const T& packet) {
    if (mMaxSize > 0 && mPackets.size() == static_cast<size_t> (mMaxSize)) {
        return;
    }
    mPackets.push(packet);
}

template <typename T>
T FifoList<T>::pop()throw(std::out_of_range) {
    if (mPackets.empty()){
        throw (std::out_of_range("Buffer is empty"));
    }
    T temp = mPackets.front();
    mPackets.pop();
    return (temp);
}

#endif	/* PACKERTBUFFER_H */

