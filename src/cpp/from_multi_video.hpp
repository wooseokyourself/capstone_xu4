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

void
handle_thread (int& camId, std::vector<cv::Mat>& imgs, bool& picture_flag, int& MODE_FLAG, std::mutex& m);

void
camera_handler (io_data& _io_data, config_data& _conf_data, int& WORK_FLAG, int& MODE_FLAG, std::mutex& m);

#endif
