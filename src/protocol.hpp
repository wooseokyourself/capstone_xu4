/*
    이 파일은 클라이언트, 서버와 항상 동기화되어있어야한다.
*/

#ifndef PROTOCOL
#define PROTOCOL

#include <cstdint>
#include <vector>

struct protocol {
    size_t bufSize;
    std::vector<unsigned char> buf;
};

#endif
