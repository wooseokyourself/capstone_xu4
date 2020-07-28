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
    uploadIPs (const vector<string>& clntAddrs);

    void
    uploadInput (const IOdata& ioData);

    void
    uploadOutput (const IOdata& ioData);

};

#endif
