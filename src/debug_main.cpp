#include "ml.hpp"
#include "web.hpp"

int main (int argc, char* argv[]) {
    if (argc != 3) {
        printf ("./dnn.out <처음: 테스트할 이미지 파일> <끝: 테스트할 이미지 파일>\n");
        return 0;
    }
    int imgStart = atoi(argv[1]);
    int imgEnd = atoi(argv[2]);
    OpenCV_DNN dnn;
    Uploader web;
    for (int i=imgStart; i<=imgEnd; i++) {
        string TEST_IMAGE_FILE =  to_string(i) + ".jpeg";
        dnn.MachineLearning (TEST_IMAGE_FILE);
        web.upload (dnn.getResultImg(), dnn.getPeopleNumber(), TEST_IMAGE_FILE);
    }
    return 0;
}
