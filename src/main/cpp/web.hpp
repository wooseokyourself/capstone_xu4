#ifndef WEB
#define WEB

#include "common.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;
using namespace std;

class Uploader {
public:
    Uploader(string WEB_ROOT);

    void
    upload_input (Mat inputImg, string fileName);

    void
    upload_output (Mat resultImg, int peopleNumber, string fileName);

private:
    string INPUT_IMG_SAVE_PATH;
    string OUTPUT_IMG_SAVE_PATH;
    char* results_PATH;
};

#endif
