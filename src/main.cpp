#include "common.hpp"

#include "ml.hpp"
#include "socket.hpp"


int main (void) {
    OpenCV_DNN dnn;
    while (true) {
        std::vector<unsigned char> dataPtr = RecvBuffer ();
		dnn.MachineLearning (dataPtr);
    }
    return 0;
}
