#ifndef ML
#define ML

#include "common.hpp"

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

class OpenCV_DNN {
public:

    OpenCV_DNN();

    inline Mat
    getResultImg () { return outputImg; }

    inline int
    getPeopleNumber () { return people; }

    void 
    MachineLearning (vector<unsigned char> vec);

protected:

    inline void
    preprocess (const Mat& frame);
    
    void
    postprocess (Mat& frame, const vector<Mat>& outs);

    void
    drawPred (int classId, float conf, int left, int top, int right, int bottom, Mat& frame);
    

private: // 파일 및 출력정보

    Mat outputImg;
    int people;
    string fileName; // YYYYMMDD.jpeg


private: // 딥러닝 변수

    string MODEL_PATH;
    string CONFIG_PATH;
    string CLASSES_PATH;
    string INPUT_IMAGE_PATH;

    Net net;

    Scalar mean;

    float scale;
    bool swapRB;
    int inpWidth;
    int inpHeight;

    float confThreshold;
    float nmsThreshold;

    vector<string> classes;
    vector<cv::String> outNames;
};

#endif
