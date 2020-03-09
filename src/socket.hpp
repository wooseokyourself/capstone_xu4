#ifndef SOCKET
#define SOCKET

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <vector>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#define ASSERT assert

#define MAXBUFSIZE 512
#define PORT    10051

static const int MAXPENDING = 1;    // 연결 요청을 대기할 수 있는 최대 수

ssize_t
Recv (int sock, const void *buf, ssize_t size, ssize_t unit);

std::vector<unsigned char>
RecvBuffer ();

#endif
