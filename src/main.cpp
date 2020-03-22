#include "socket.hpp"
#include "ml.hpp"
#include "web.hpp"


int main (int argc, char* argv[]) {
    if (argc != 2) {
        printf ("./server_debug.out $(WEB_ROOT)\n");
        return 0;
    }
    string WEB_ROOT = argv[1];
    OpenCV_DNN dnn;
    Uploader web(WEB_ROOT);
    while (true) {
        std::vector<unsigned char> dataPtr = RecvBuffer ();
        Mat inputImg = imdecode (dataPtr, 1);
        dataPtr.clear();

        string currTime = getCurrTime();
        web.upload_input (inputImg, currTime + ".jpeg");
		dnn.MachineLearning (inputImg);
        web.upload_output (dnn.getOutputImg(), dnn.getPeopleNumber(), currTime + ".jpeg");
    }
    return 0;
}
