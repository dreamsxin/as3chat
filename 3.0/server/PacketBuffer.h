/*
 * File:   PacketBuffer.h
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
class PacketBuffer {
public:
    PacketBuffer(int maxSize = -1);
    void bufferPacket(const T&packer);
    T getNextPacket() throw (std::out_of_range);
protected:
    queue<T> mPackets;
    int mMaxSize;
private:
    PacketBuffer(const PacketBuffer& src);
    PacketBuffer & operator=(const PacketBuffer& rhs);
};

template <typename T>
PacketBuffer<T>::PacketBuffer(int maxSize) {
    mMaxSize = maxSize;
}

template <typename T>
void PacketBuffer<T>::bufferPacket(const T& packet) {
    if (mMaxSize > 0 && mPackets.size() == static_cast<size_t> (mMaxSize)) {
        return;
    }
    mPackets.push(packet);
}

template <typename T>
T PacketBuffer<T>::getNextPacket()throw(std::out_of_range) {
    if (mPackets.empty()){
        throw (std::out_of_range("Buffer is empty"));
    }
    T temp = mPackets.front();
    mPackets.pop();
    return (temp);
}

#endif	/* PACKERTBUFFER_H */

