#ifndef ML
#define ML

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

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

#ifdef DIFF_BASE
struct Decoded {
    Mat prev;
    Mat curr;
    uint32_t diffValue;
};
#endif

class OpenCV_DNN {
public:

    OpenCV_DNN();

    void 
    MachineLearning (struct protocol* dataPtr);

#ifdef DEBUG_ML
    void
    MachineLearning (string TEST_IMAGE_PATH);
#endif

protected:

#ifdef DIFF_BASE
    struct Decoded*
    decoding (struct protocol* dataPtr);
#else
    Mat
    decoding (struct protocol* dataPtr);
#endif

    inline void
    preprocess (const Mat& frame);
    
    void
    postprocess (Mat& frame, const vector<Mat>& outs);

    void
    drawPred (int classId, float conf, int left, int top, int right, int bottom, Mat& frame);
    
    inline string
    getCurrTime ();

private:

    string MODEL_PATH;
    string CONFIG_PATH;
    string CLASSES_PATH;
    string INPUT_IMAGE_PATH;
    string OUTPUT_IMAGE_PATH;

    Net net;

    Scalar mean;

    float scale;
    bool swapRB;
    int inpWidth;
    int inpHeight;

    float confThreshold;
    float nmsThreshold;

    vector<string> classes;
    vector<String> outNames;
};



#endif
