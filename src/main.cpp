#include "socket.hpp"
#include "ml.hpp"
#include "web.hpp"


int main (void) {
    OpenCV_DNN dnn;
    Uploader web;
    while (true) {
        std::vector<unsigned char> dataPtr = RecvBuffer ();
        Mat inputImg = imdecode (dataPtr, 1);
        dataPtr.clear();

        /* 입력 및 출력이미지는 YYYYMMDDHHMMSS.jpeg 형태로 저장 */
        string currTime = getCurrTime();
        web.upload_input (inputImg, currTime + ".jpeg");
		dnn.MachineLearning (inputImg);
        web.upload_output (dnn.getOutputImg(), dnn.getPeopleNumber(), currTime + ".jpeg");
    }
    return 0;
}