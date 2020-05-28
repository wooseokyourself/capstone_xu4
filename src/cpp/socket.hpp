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
send_mode_flag (const int& clntSock, int& MODE_FLAG);

void
send_res (const int& clntSock, cv::Size& res);

void
send_notification (const int& clntSock);

void
handle_thread (const int& clntSock, std::vector<cv::Mat>& imgs, cv::Size& res, bool& picture_flag, int& MODE_FLAG, std::mutex& m);

void
camera_handler (io_data& _io_data, config_data& _conf_data, int& WORK_FLAG, int& MODE_FLAG, std::mutex& m);

#endif
