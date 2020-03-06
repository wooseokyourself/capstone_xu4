/*
    이 파일은 클라이언트, 서버와 항상 동기화되어있어야한다.
*/

#ifndef PROTOCOL
#define PROTOCOL

#include <stdint.h>
#include <vector>

#define PROTO_BUFSIZE sizeof(uint64_t)

using namespace std;

struct protocol {
    vector<unsigned char> prevBuf;
    vector<unsigned char> currBuf;
    uint32_t diffValue; // abs(prevImg - currImg)
};

#endif
