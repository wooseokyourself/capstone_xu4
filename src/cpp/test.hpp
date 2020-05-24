#ifndef TEST
#define TEST

#include "common.hpp"

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define GO_TAKE_PICTURE 0
#define UNTIL_TAKE_PICTURE 1
#define DONE_TAKE_PICTURE 2
#define GO_INFERENCE 3

#define MAXBUFSIZE 512
#define PORT    10001

void
handle_thread (const string& path, const int& camId, std::vector<cv::Mat>& imgs, bool& picture_flag, int& MODE_FLAG, std::mutex& m);

void
camera_handler (std::vector<cv::Mat>& imgs, const int& totalCam, int& WORK_FLAG, int& MODE_FLAG, std::mutex& m);

#endif