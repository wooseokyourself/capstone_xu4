#ifndef UPLOADER
#define UPLOADER

#include "common.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;
using namespace std;

class Uploader {
public:

    void
    upload_input (const io_data& _io_data);

    void
    upload_output (const io_data& _io_data);

};

#endif
