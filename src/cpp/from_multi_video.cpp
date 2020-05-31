#include "from_multi_video.hpp"

/* Receive pictures in each thread. */
void
handle_thread (int& camId, cv::VideoCapture& cap, std::vector<cv::Mat>& imgs, bool& picture_flag, int& MODE_FLAG, std::mutex& m) {
    int dummy;
    int recvd;
    cv::Mat frame;
    // Receive id of cam
    while (cap.read(frame)) {
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        if (!picture_flag) { // 사진을 가져오라는 명령이 떨어짐
            // 스레드별로 camId를 먼저 수신
            // 이후 데이터를 수신하고 디코딩하여 imgs[camId-1] 에 저장.
            // imgs[camId-1] 저장이 끝난 스레드는 종료.


            imgs[camId] = frame.clone(); // Decode bytes into Mat class image.

            m.lock();
            picture_flag = true; // 사진수신을 완료하였음을 알림
            m.unlock();
            printf ("<%d's camera sent a picture completely!>\n", camId+1);
        }
        else {// 사진수신할 필요가 없으므로 대기
            dummy++;
        }
    }
}

void
camera_handler (io_data& _io_data, config_data& _conf_data, int& WORK_FLAG, int& MODE_FLAG, std::mutex& m) {
    const int& camera_number = _conf_data.camera_number;
    std::vector<cv::VideoCapture> videos (camera_number);
    int connected_number = 0;
    
    while (connected_number < camera_number) { // 동영상 열기
        videos[connected_number] = VideoCapture(BIN_PATH + "/testvideos/" + to_string(connected_number+1) + ".mp4");
        if (videos[connected_number].isOpened())
            printf ("Client connected: %d\n", connected_number+1);
        else
            puts ("Unable to get client address");
        connected_number++;
    }

    std::thread* thrs = new std::thread[camera_number];
    bool* picture_flag = new bool[camera_number]; // 여기 스레드에서 각 스레드별 사진수신여부를 총합하는 플래그
    for (int i=0; i<camera_number; i++) {
        picture_flag[i] = false; // i번째 스레드의 사진이 수신되었으면 true로 변경됨
        // printf ("[thread %d] created!\n", i);
        thrs[i] = std::thread(handle_thread, std::ref(i), std::ref(videos[i]), std::ref(_io_data.imgs), std::ref(picture_flag[i]), std::ref(MODE_FLAG), std::ref(m));
    }

    // 각 스레드를 총괄하는 루프
    int dummy = 0;
    while (true) { // 초기 스레드들을 배분하면 이후에는 프로그램이 종료될때까지 여기에서 머뭄
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        if (WORK_FLAG == GO_TAKE_PICTURE) { // 사진을 가져오라는 명령이 떨어짐

            for (int i=0; i<camera_number; i++)
                picture_flag[i] = false; // 각 스레드들에게 사진 수신하라고 알리기
            
            while (true) { // 각 스레드 사진수신 완료되었는지 조사
                bool go_to_next_work = true;
                for (int i=0; i<camera_number; i++)
                    if (!picture_flag[i]) // 아직 사진이 수신되지 않은 스레드가 있다면
                        go_to_next_work = false;
                if (go_to_next_work) // 모든 사진이 다 수신되었다면
                    break;
            }
            // 모든 스레드들이 사진수신을 완료하였으므로
            printf (" WORK_FLAG: GO_TAKE_PICTURE --> DONE_TAKE_PICTURE\n");
            m.lock();
            WORK_FLAG = DONE_TAKE_PICTURE; // 이를 확인한 메인스레드는 Mat* imgs를 입력으로 딥러닝 시행
            m.unlock();
        }
        else { // 현재 딥러닝중이므로 대기
            dummy++;
        }
    }

    for (int i=0; i<camera_number; i++) {
        thrs[i].join(); // 각 스레드에서 handle_thread() 가 리턴될때까지 대기 (사진을 찍을 때까지 대기)
    }
    delete[] thrs;
    delete[] picture_flag;
}
