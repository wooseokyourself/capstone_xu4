#ifndef ADMIN
#define ADMIN

#include "common.hpp"

class config_data {
public:

    string CONFIG_PATH;
    int camera_number;
    Size capture_res;
    Size resize_res;
    float confThreshold; // default=0.4
    float nmsThreshold; // default=0.5
    std::vector< pair<int, string> > overlapping_area;

public:
    
    config_data (string CONFIG_PATH);

    void
    read ();
    
};

#endif