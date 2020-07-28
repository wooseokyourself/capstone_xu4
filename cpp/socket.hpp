#ifndef SOCKET
#define SOCKET

#include "common.hpp"
#include "admin.hpp"

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
#define PORT 10001

// Maximum number of requests to wait for a connection.
static const int MAXPENDING = 10;

ssize_t
Recv (const int& sock, const void *buf, ssize_t size, ssize_t unit);

void
sendModeFlag (const int& clntSock, int& MODE_FLAG);

void
sendRes (const int& clntSock, int& width, int& height);

void
sendNotification (const int& clntSock);

void
handleThread (const int& clntSock, std::vector<cv::Mat>& imgs, 
                int& width, int& height, bool& pictureFlag, 
                int& MODE_FLAG, std::mutex& m);

void
cameraHandler (IOdata& ioData, ConfigData& confData, 
                std::vector<string>& clntAddrs, bool& allConnected, 
                int& WORK_FLAG, int& MODE_FLAG, std::mutex& m);

#endif
