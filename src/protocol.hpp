/*
    이 파일은 클라이언트, 서버와 항상 동기화되어있어야한다.
*/

#ifndef PROTOCOL
#define PROTOCOL

#include <cstdint>
#include <vector>

//#define PROTO_BUFSIZE sizeof(uint64_t)
#define PROTO_BUFSIZE sizeof(size_t)

using namespace std;

#ifdef DIFF_BASE
struct protocol {
    size_t bufSize;
    vector<unsigned char> prevBuf;
    vector<unsigned char> currBuf;
    uint32_t diffValue; // abs(prevImg - currImg)
};
#else
struct protocol {
    size_t bufSize;
    vector<unsigned char> buf;
};
#endif

#endif
