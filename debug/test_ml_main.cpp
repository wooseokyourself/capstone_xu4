#include "../src/ml.hpp"

int main (int argc, char* argv[]) {
    if (argc != 2) {
        printf ("./dnn.out <테스트할 이미지 파일의 수>\n");
        return 0;
    }
    int imgNum = atoi(argv[1]);
    OpenCV_DNN dnn;
    for (int i=1; i<=imgNum; i++) {
        string TEST_IMAGE_PATH = "debug/test_images/" + to_string(i) + ".jpeg";
        dnn.MachineLearning (TEST_IMAGE_PATH);
    }
    return 0;
}
