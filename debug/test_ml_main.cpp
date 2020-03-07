#include "../src/ml.hpp"

int main (void) {
    OpenCV_DNN dnn;
    for (int i=1; i<=5; i++) {
        string TEST_IMAGE_PATH = "debug/test_images/" + to_string(i) + ".jpeg";
        dnn.MachineLearning (TEST_IMAGE_PATH);
    }
    return 0;
}
