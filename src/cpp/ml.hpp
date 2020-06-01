#ifndef ML
#define ML

#include "common.hpp"
#include "admin.hpp"

using namespace std;
using namespace cv;
using namespace dnn;


class OpenCV_DNN {
public:

    OpenCV_DNN(const config_data& data);

    void
    update (const config_data& data);

    void
    inference (io_data& _io_data);

protected:

    int 
    infer_util (const int& idx, Mat& img);

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
    Size resize_res;
    float confThreshold;
    float nmsThreshold;
    vector< vector<int> > ovlaps;

    // Model Info
    vector<string> classes;
    vector<cv::String> outNames;
};

#endif
