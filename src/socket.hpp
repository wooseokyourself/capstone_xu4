#ifndef SOCKET
#define SOCKET

#include "common.hpp"

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define MAXBUFSIZE 512
#define PORT    10051

// Maximum number of requests to wait for a connection.
static const int MAXPENDING = 1;

ssize_t
Recv (int sock, const void *buf, ssize_t size, ssize_t unit);

vector<unsigned char>
RecvBuffer ();

#endif
