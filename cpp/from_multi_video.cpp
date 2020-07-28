#include "from_multi_video.hpp"

/* Receive pictures in each thread. */
void
handleThread (int camId, std::vector<cv::Mat>& imgs, bool& pictureFlag, int& MODE_FLAG, std::mutex& m) {
    int recvd;
    camId += 1;
    printf ("Finding video file is %d.mp4\n", camId);
    cv::VideoCapture cap(BIN_PATH + "/testvideos/" + to_string(camId) + ".mp4");
    if (cap.isOpened())
        printf ("Client connected: %d\n", camId);
    // Receive id of cam
    while (true) {
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        Mat frame;
        cap >> frame;

        if (frame.empty()) {
            printf ("frame is empty!\n");
            break;
        }

        if (!pictureFlag) { // 사진을 가져오라는 명령이 떨어짐
            m.lock();
            imgs[camId-1] = frame.clone();
            pictureFlag = true; // 사진수신을 완료하였음을 알림
            m.unlock();
            
            printf ("<%d's camera sent a picture completely!>\n", camId);
        }
    }
    cap.release();
}

void
cameraHandler (IOdata& ioData, ConfigData& confData, 
                std::vector<string>& clntAddrs, bool& allConnected,
                int& WORK_FLAG, int& MODE_FLAG, std::mutex& m) {
    const int& cameraNumber = confData.cameraNumber;
    clntAddrs.resize (cameraNumber);
    allConnected = false;

    std::thread* thrs = new std::thread[cameraNumber];
    bool* pictureFlag = new bool[cameraNumber]; // 여기 스레드에서 각 스레드별 사진수신여부를 총합하는 플래그
    for (int i=0; i<cameraNumber; i++) {
        pictureFlag[i] = false; // i번째 스레드의 사진이 수신되었으면 true로 변경됨
        // printf ("[thread %d] created!\n", i);
        printf ("create thread, args of camId=%d\n", i);
        clntAddrs[i] = to_string(i+1) + ".mp4";
        thrs[i] = std::thread(handleThread, i, std::ref(ioData.imgs), std::ref(pictureFlag[i]), std::ref(MODE_FLAG), std::ref(m));
    }
    allConnected = true;

    // 각 스레드를 총괄하는 루프
    while (true) { // 초기 스레드들을 배분하면 이후에는 프로그램이 종료될때까지 여기에서 머뭄
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        if (WORK_FLAG == GO_TAKE_PICTURE) { // 사진을 가져오라는 명령이 떨어짐

            for (int i=0; i<cameraNumber; i++)
                pictureFlag[i] = false; // 각 스레드들에게 사진 수신하라고 알리기
            
            while (true) { // 각 스레드 사진수신 완료되었는지 조사
                bool goToNextWork = true;
                for (int i=0; i<cameraNumber; i++)
                    if (!pictureFlag[i]) // 아직 사진이 수신되지 않은 스레드가 있다면
                        goToNextWork = false;
                if (goToNextWork) // 모든 사진이 다 수신되었다면
                    break;
            }
            // 모든 스레드들이 사진수신을 완료하였으므로
            printf (" WORK_FLAG: GO_TAKE_PICTURE --> DONE_TAKE_PICTURE\n");
            m.lock();
            WORK_FLAG = DONE_TAKE_PICTURE; // 이를 확인한 메인스레드는 Mat* imgs를 입력으로 딥러닝 시행
            m.unlock();
        }
    }

    for (int i=0; i<cameraNumber; i++) {
        thrs[i].join(); // 각 스레드에서 handleThread() 가 리턴될때까지 대기 (사진을 찍을 때까지 대기)
    }
    delete[] thrs;
    delete[] pictureFlag;
}
