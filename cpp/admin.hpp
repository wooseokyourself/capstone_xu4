#ifndef ADMIN
#define ADMIN

#include "common.hpp"

using namespace cv;

typedef struct _AdminData {
    int cameraNumber;
    int captureResWidth;
    int captureResHeight;
    int resizeResWidth;
    int resizeResHeight;
} AdminData;

class ConfigData {
public:

    int prevFlag;
    int cameraNumber;
    int captureResWidth;
    int captureResHeight;
    int resizeResWidth;
    int resizeResHeight;
    float confThreshold; // default=0.4
    float nmsThreshold; // default=0.5
    std::vector< std::vector< int > > overlaps;

public:
    configData ();

    bool
    sync ();
/*
    bool
    sync (bool isFirstCall);

protected:
    int
    readModeFlag ();
    void
    readAdminInput ();
    void
    readOverlaps ();
*/
};

#endif