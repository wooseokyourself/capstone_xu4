#ifndef ADMIN
#define ADMIN

#include "common.hpp"

using namespace cv;

class config_data {
public:

    int prev_flag;
    int camera_number;
    cv::Size capture_res;
    cv::Size resize_res;
    float confThreshold; // default=0.4
    float nmsThreshold; // default=0.5
    std::vector< std::pair<int, string> > overlapping_area;

public:
    config_data ();
    bool
    sync ();

protected:
    int
    read_mode_flag ();
    void
    read_all_config ();

};

#endif