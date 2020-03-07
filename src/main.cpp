#include <string>

#include "web.hpp"
#include "ml.hpp"
#include "socket.hpp"


int main (void) {
    OpenCV_DNN dnn;
    while (true) {
        struct protocol* dataPtr = RecvBuffer ();
		dnn.MachineLearning (dataPtr);
    }
    return 0;
}
