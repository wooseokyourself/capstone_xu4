#ifndef ADMIN
#define ADMIN

#include "common.hpp"

using namespace cv;

class config_data {
public:

    int prev_flag;
    int camera_number;
    int capture_res_width;
    int capture_res_height;
    int resize_res_width;
    int resize_res_height;
    float confThreshold; // default=0.4
    float nmsThreshold; // default=0.5
    std::vector< std::vector< int > > ovlaps;

public:
    config_data ();
    bool
    sync (bool is_first_call);

protected:
    int
    read_mode_flag ();
    void
    read_admin_input ();
    void
    read_ovlaps ();

};

#endif