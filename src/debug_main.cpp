#include "ml.hpp"
#include "web.hpp"

int main (int argc, char* argv[]) {
    if (argc != 3) {
        printf ("./server_debug.out $(WEB_ROOT) $(LAST)\n");
        return 0;
    }
    string WEB_ROOT = argv[1];
    // Check the last element of @WEB_ROOT is '/'
    ASSERT (WEB_ROOT[WEB_ROOT.length()-1] == '/');
    
    int imgEnd = atoi(argv[2]);
    ASSERT (imgEnd <= 99);

    OpenCV_DNN dnn;
    Uploader web(WEB_ROOT);
    for (int i=1; i<=imgEnd; i++) {
        string TEST_IMAGE_NAME = to_string(i) + ".jpeg";
        Mat inputImg = imread("debug/images/" + TEST_IMAGE_NAME, IMREAD_COLOR);
        if (TEST_IMAGE_NAME.length() == 6) {
            TEST_IMAGE_NAME = "0" + TEST_IMAGE_NAME;
        }
        web.upload_input (inputImg, TEST_IMAGE_NAME);
        dnn.MachineLearning (inputImg);
        web.upload_output (dnn.getOutputImg(), dnn.getPeopleNumber(), TEST_IMAGE_NAME);
    }
    return 0;
}
