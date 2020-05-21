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
#define PORT    10001

// Maximum number of requests to wait for a connection.
static const int MAXPENDING = 10;

ssize_t
Recv (const int& sock, const void *buf, ssize_t size, ssize_t unit);

void
send_notification (const int& clntSock);

void
send_terminate_flag (const int& clntSock, bool& terminate_flag);

void
send_notification (const int& clntSock);

void
handle_thread (const int& clntSock, std::vector<cv::Mat>& imgs, bool& picture_flag, int& MODE_FLAG, std::mutex& m);

void
camera_handler (std::vector<cv::Mat>& imgs, const int& totalCam, int& WORK_FLAG, int& MODE_FLAG, std::mutex& m);

#endif
