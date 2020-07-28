#ifndef ML
#define ML

#include "common.hpp"
#include "admin.hpp"

using namespace std;
using namespace cv;
using namespace dnn;


class OpenCV_DNN {
public:

    OpenCV_DNN(const ConfigData& data);

    void
    update (const ConfigData& data);

    void
    inference (IOdata& ioData);

protected:

    int 
    inferUtil (const int& idx, Mat& img);

    inline void
    preprocess (const int& idx, Mat& frame);
    
    int
    postprocess (Mat& frame, const vector<Mat>& outs);

    void
    removeOverlaps (const int& idx, Mat& frame);

    void
    imagePadding (Mat& frame);

    void
    drawPred (int classId, float conf, int left, int top, int right, int bottom, Mat& frame);

private: // Variables for Machine Learning.

    string MODEL_PATH;
    string CFG_PATH;
    string CLASSES_PATH;
    string INPUT_IMAGE_PATH;

    Net net;

    // DNN Info
    Scalar mean;
    double scale;
    double scalarfactor;
    bool swapRB;

    // Admin Mode Config Data
    Size resizeRes;
    float confThreshold;
    float nmsThreshold;
    vector< vector<int> > overlaps;

    // Model Info
    vector<string> classes;
    vector<cv::String> outNames;
};

#endif
