#ifndef WEB
#define WEB

#include "common.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;
using namespace std;

class Uploader {
public:

    Uploader(string ROI_DIR_PATH, string RESULT_DIR_PATH);

    void
    upload_input (const io_data& _io_data);

    void
    upload_output (const io_data& _io_data, string file_name = getCurrTime());

private:

    string ROI_DIR_PATH;
    string RESULT_DIR_PATH;
    char* results_PATH;
};

#endif
