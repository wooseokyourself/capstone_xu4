#include "common.hpp"

#include "ml.hpp"
#include "socket.hpp"


int main (void) {
    OpenCV_DNN dnn;
    while (true) {
        std::vector<unsigned char> dataPtr = RecvBuffer ();
        Mat inputimg = imdecode (dataPtr, IMREAD_UNCHANGED);
        dataPtr.clear();
		dnn.MachineLearning (inputImg);
    }
    return 0;
}