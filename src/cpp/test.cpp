#include "test.hpp"

void
handle_thread (const string& path, const int& camId, std::vector<cv::Mat>& imgs, bool& picture_flag, int& MODE_FLAG, std::mutex& m) {
    VideoCapture cap(path);
    ASSERT (cap.isOpened() == true);
    Mat frame;
    cap >> frame;
    int dummy;
    while (true) { // 베이직 모드라면 이 스레드 계속 실행
        cap >> frame;
        if (MODE_FLAG == BASIC_MODE) {
            if (!picture_flag) { // 사진을 가져오라는 명령이 떨어짐

                // 재생중인 동영상에서 캡쳐하기
                imgs[camId-1] = frame.clone();

                m.lock();
                picture_flag = true; // 사진수신을 완료하였음을 알림
                m.unlock();
                printf ("<%d's camera sent a picture completely!>\n", camId);
            }
            else {// 사진수신할 필요가 없으므로 대기
                dummy++;
            }
        }
        else if (MODE_FLAG == ADMIN_MODE) {
            // 사진 찍지 말고 대기
            dummy++;
        }
        else { // TERMINATE_MODE
            break;
        }
    }
}

void
camera_handler (std::vector<cv::Mat>& imgs, const int& totalCam, int& WORK_FLAG, int& MODE_FLAG, std::mutex& m) {
    // 두 개의 동영상을 동시 재생하기 위한 스레드 할당
    std::thread thrs[2];
    bool picture_flag[2];
    picture_flag[0] = false;
    picture_flag[1] = false;
    thrs[0] = std::thread(handle_thread, (P::BIN_PATH + "/testvideos/1.mp4"), 1, std::ref(imgs), std::ref(picture_flag[0]), std::ref(MODE_FLAG), std::ref(m));
    thrs[1] = std::thread(handle_thread, (P::BIN_PATH + "/testvideos/2.mp4"), 2, std::ref(imgs), std::ref(picture_flag[1]), std::ref(MODE_FLAG), std::ref(m));

    int dummy = 0;
    while (true) { // 초기 스레드들을 배분하면 이후에는 프로그램이 종료될때까지 여기에서 머뭄
        if (MODE_FLAG == BASIC_MODE) {
            if (WORK_FLAG == GO_TAKE_PICTURE) { // 사진을 가져오라는 명령이 떨어짐

                for (int i=0; i<2; i++)
                    picture_flag[i] = false; // 각 스레드들에게 사진 수신하라고 알리기
                
                while (true) { // 각 스레드 사진수신 완료되었는지 조사
                    bool go_to_next_work = true;
                    for (int i=0; i<2; i++)
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
        else if (MODE_FLAG == ADMIN_MODE) {
            // 사진찍지말고 대기
            dummy++;
        }
        else { // TERMINATE_MODE
            break;
        }

    }

    for (int i=0; i<2; i++)
        thrs[i].join();
}