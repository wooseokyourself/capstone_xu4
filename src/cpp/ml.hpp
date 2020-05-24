#ifndef ML
#define ML

#include "common.hpp"
#include "admin.hpp"

using namespace std;
using namespace cv;
using namespace dnn;


class OpenCV_DNN {
public:

    OpenCV_DNN(Size resize_res, float confThreshold, float nmsThreshold);

    void
    update (const config_data& data);

    io_data
    inference (io_data& _io_data);

protected:

    int 
    infer_util (Mat& img);

    inline void
    preprocess (Mat& frame);
    
    int
    postprocess (Mat& frame, const vector<Mat>& outs);

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

    // Admin Mode
    Size resize_res;
    float confThreshold;
    float nmsThreshold;

    // Model Info
    vector<string> classes;
    vector<cv::String> outNames;
};

#endif
