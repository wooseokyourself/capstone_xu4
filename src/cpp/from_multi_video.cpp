#include "from_multi_video.hpp"

void
handle_thread (const string& path, const int& camId, std::vector<cv::Mat>& imgs, bool& picture_flag, int& MODE_FLAG, std::mutex& m) {
    VideoCapture cap(path);
    ASSERT (cap.isOpened() == true);
    Mat frame;
    int dummy;
    printf ("handle_thread first call! picture_flag is %d\n", picture_flag);
    while (cap.read (frame)) { // 베이직 모드라면 이 스레드 계속 실행
        printf ("handle_thread's loop!\n");
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        ASSERT (frame.empty() == true);
        if (!picture_flag) { // 사진을 가져오라는 명령이 떨어짐
            printf (" handle_thread's taking picture!\n");
            // 재생중인 동영상에서 캡쳐하기
            imgs[camId-1] = frame.clone();
            printf (" frame to imgs[camId-1] clone is done!\n");
            m.lock();
            printf (" lock! in order to picture_flag <-- true\n");
            picture_flag = true; // 사진수신을 완료하였음을 알림
            printf (" unlock!\n");
            m.unlock();
            printf ("<%d's camera sent a picture completely!>\n", camId);
        }
        else {// 사진수신할 필요가 없으므로 대기
            dummy++;
        }
    }
}

void
camera_handler (std::vector<cv::Mat>& imgs, const int& totalCam, int& WORK_FLAG, int& MODE_FLAG, std::mutex& m) {
    // 동영상재생 스레드 할당
    std::thread* thrs = new std::thread[totalCam];
    bool* picture_flag = new bool[totalCam];
    for (int i=0; i<totalCam; i++) {
        picture_flag[i] = false;
        thrs[i] = std::thread(handle_thread, (BIN_PATH + "/testvideos/" + to_string(i+1) + ".mp4"), i+1,
             std::ref(imgs), std::ref(picture_flag[i]), std::ref(MODE_FLAG), std::ref(m));
    }
    

    int dummy = 0;
    while (true) { // 초기 스레드들을 배분하면 이후에는 프로그램이 종료될때까지 여기에서 머뭄
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        if (WORK_FLAG == GO_TAKE_PICTURE) { // 사진을 가져오라는 명령이 떨어짐

            for (int i=0; i<totalCam; i++)
                picture_flag[i] = false; // 각 스레드들에게 사진 수신하라고 알리기
            
            while (true) { // 각 스레드 사진수신 완료되었는지 조사
                bool go_to_next_work = true;
                for (int i=0; i<totalCam; i++)
                    if (!picture_flag[i]) // 아직 사진이 수신되지 않은 스레드가 있다면
                        go_to_next_work = false;
                if (go_to_next_work) // 모든 사진이 다 수신되었다면
                    break;
            }
            // 모든 스레드들이 사진수신을 완료하였으므로
            m.lock();
            WORK_FLAG = DONE_TAKE_PICTURE; // 이를 확인한 메인스레드는 Mat* imgs를 입력으로 딥러닝 시행
            m.unlock();
        }
        else { // 현재 딥러닝중이므로 대기
            dummy++;
        }
    }

    for (int i=0; i<totalCam; i++)
        thrs[i].join();
}