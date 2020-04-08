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

#define GO_TAKE_PICTURE 0
#define UNTIL_TAKE_PICTURE 1
#define DONE_TAKE_PICTURE 2
#define GO_INFERENCE 3

#define MAXBUFSIZE 512
#define PORT    10051

// Maximum number of requests to wait for a connection.
static const int MAXPENDING = 1;

ssize_t
Recv (int sock, const void *buf, ssize_t size, ssize_t unit);

void
Send_notification (int clntSock);

ssize_t
handle_cam (int clntSock, Mat* imgs);

void
RecvBuffer (Mat* imgs, int camNumber, bool& workload, bool& terminate);

#endif
