#include "socket.hpp"
#include "ml.hpp"
#include "web.hpp"


int main (int argc, char* argv[]) {
    if (argc != 2) {
        printf ("./server_debug.out $(WEB_ROOT)\n");
        return 0;
    }
    string WEB_ROOT = argv[1];
    // Check the last element of @WEB_ROOT is '/'
    ASSERT (WEB_ROOT[WEB_ROOT.length()-1] == '/');

    OpenCV_DNN dnn;
    Uploader web(WEB_ROOT);

    int totalCam = 0;
    printf ("Input the number of camera: ");
    scanf ("%d", &totalCam);
    // Each cam is identified with its id-1 in the index of arrays.
    std::vector<cv::Mat> imgs(totalCam);
    std::vector<cv::Mat> outImgs(totalCam);
    std::mutex m;
    int workload = GO_TAKE_PICTURE;
    bool terminate_flag = false;
    std::thread tcp_thr (RecvBuffer, std::ref(imgs), std::ref(totalCam), std::ref(workload), std::ref(terminate_flag), std::ref(m)); // 소켓통신 시작
    int dummy = 0;
    while (true) {
        if (workload == DONE_TAKE_PICTURE) { // 사진촬영을 모두 완료하였다면
            m.lock();
            workload = GO_INFERENCE;
            m.unlock();

            /* 여기에서 모든 imgs에 대해 각각 MachineLearning 을 적용하여야함. */
            string currTime = getCurrTime();
            int totalPeopleNum = 0;
            for (int i=0; i<totalCam; i++) {
                web.upload_input (imgs[i], currTime + "__" + to_string(i) + ".jpeg");
                dnn.MachineLearning (imgs[i]);
                outImgs[i] = dnn.getOutputImg();
                totalPeopleNum += dnn.getPeopleNumber();
                imgs[i].release();
            }
            cv::Mat mergedOut(outImgs[0].rows/2, outImgs[0].cols, outImgs[0].type(), cv::Scalar(255, 255, 255));
            // 이 패딩 이미지에 @totalPeopleNum 먼저 출력하기.
            for (int i=0; i<totalCam; i++) {
                cv::hconcat (mergedOut, outImgs[i], mergedOut); // 모든 이미지들을 가로로 붙이기.
                outImgs[i].release();
            }
            web.upload_output (mergedOut, totalPeopleNum, currTime + ".jpeg");
            mergedOut.release();

            m.lock();
            workload = GO_TAKE_PICTURE; // 다시 사진촬영 요청
            m.unlock();
        }
        else // 사진을 촬영하고 있으므로 대기
            dummy++;
        
    }
    imgs.clear();
    outImgs.clear();
    return 0;
}
