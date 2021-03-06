#ifndef COMMON
#define COMMON

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <cstring>

#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <thread>
#include <mutex>

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>


#define ASSERT assert

#define ADMIN_MODE 0
#define BASIC_MODE 1
#define TERMINATE_MODE 2

using std::string;

string
_ROOT__PATH();

string
_CONFIG__PATH();

string
_BIN__PATH();

string
_ROI__DIR__PATH();

string
_RESULT__DIR__PATH();

#define ROOT_PATH _ROOT__PATH()
#define CONFIG_PATH _CONFIG__PATH()
#define BIN_PATH _BIN__PATH()
#define ROI_DIR_PATH _ROI__DIR__PATH()
#define RESULT_DIR_PATH _RESULT__DIR__PATH()

string 
getCurrTime ();

class IOdata {
public:
    int cameraNum;
    std::vector<cv::Mat> imgs; // output images
    std::vector<int> nums; // each outp's people number
    int totalPeopleNum; // all outp's total people number 
    double inferenceTime; // total inference time
public:
    IOdata (int cameraNum);
    ~IOdata ();
    void clear ();
};

#endif
