#include "../src/ml.hpp"

int main (int argc, char* argv[]) {
    if (argc != 3) {
        printf ("./dnn.out <처음: 테스트할 이미지 파일> <끝: 테스트할 이미지 파일>\n");
        return 0;
    }
    int imgStart = atoi(argv[1]);
    int imgEnd = atoi(argv[2]);
    OpenCV_DNN dnn;
    for (int i=imgStart; i<=imgEnd; i++) {
        string TEST_IMAGE_PATH = "debug/test_images/" + to_string(i) + ".jpeg";
        dnn.MachineLearning (TEST_IMAGE_PATH);
    }
    return 0;
}