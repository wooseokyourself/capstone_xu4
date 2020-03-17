#include "ml.hpp"
#include "web.hpp"

int main (int argc, char* argv[]) {
    if (argc != 3) {
        printf ("./server_debug.out <처음: 테스트할 이미지 파일> <끝: 테스트할 이미지 파일>\n");
        return 0;
    }
    int imgStart = atoi(argv[1]);
    int imgEnd = atoi(argv[2]);
    OpenCV_DNN dnn;
    Uploader web;
    for (int i=imgStart; i<=imgEnd; i++) {
        string TEST_IMAGE_NAME = to_string(i) + ".jpeg";
        Mat inputImg = imread("debug/test_images/" + TEST_IMAGE_NAME, IMREAD_COLOR);
        if (TEST_IMAGE_NAME.length() == 6) {
            TEST_IMAGE_NAME = "0" + TEST_IMAGE_NAME;
        }
        web.upload_input (inputImg, TEST_IMAGE_NAME);
        dnn.MachineLearning (inputImg);
        web.upload_output (dnn.getOutputImg(), dnn.getPeopleNumber(), TEST_IMAGE_NAME);
    }
    return 0;
}
