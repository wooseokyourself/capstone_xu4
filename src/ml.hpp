#ifndef ML
#define ML

#include <cstdio>
#include <cassert>

#include <iostream>
#include <fstream>
#include <sstream>

#ifdef DEBUG
#include <ctime>
#include <string>
#endif

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "protocol.hpp"

#define ASSERT assert

using namespace std;
using namespace cv;
using namespace dnn;

struct decoded {
    Mat prev;
    Mat curr;
    uint32_t diffValue;
};


struct decoded*
decoding (struct protocol* dataPtr);

bool
setNet (Net& net);

inline void
preprocess (const Mat& frame, Net& net, Size inpSize, float scale,
            const Scalar& mean, bool swapRB);

void
postprocess (Mat& frame, const std::vector<Mat>& out, Net& net);

void
drawPred (int classId, float conf, int left, int top, int right, int bottom, Mat& frame);

void 
MachineLearning (struct protocol* dataPtr);

#endif
