#include "from_single_video.hpp"
#include "ml.hpp"
#include "uploader.hpp"
#include "admin.hpp"

/* 어플 최초 실행시 conf_data를 웹에서 입력받은 뒤에 이 C++ 프로그램이 실행되어야 config data를 초기화할 수 있음 */
int
main (int argc, char* argv[]) {
    printf ("start program\n");
    config_data conf_data;
    printf ("conf sync!\n");
    // conf_data.sync(); // 서비스 최초 실행시 제일 먼저 conf_data를 읽어온다.
    conf_data.camera_number = 1;
    conf_data.capture_res.width = 416;
    conf_data.capture_res.height = 416;
    conf_data.resize_res.width = 416;
    conf_data.resize_res.height = 416;
    printf ("conf sync done!\n");
    OpenCV_DNN dnn (conf_data.resize_res, conf_data.confThreshold, conf_data.nmsThreshold);
    printf ("dnn done !\n");
    Uploader ups;

    io_data _io_data (conf_data.camera_number);

    int MODE_FLAG = BASIC_MODE;
    int WORK_FLAG = GO_TAKE_PICTURE;

    std::mutex m;
    printf ("1\n"); 
    std::thread cam_thr (camera_handler, std::ref(_io_data.imgs), std::ref(conf_data.camera_number), 
                        std::ref(WORK_FLAG), std::ref(MODE_FLAG), std::ref(m)); // 소켓통신 시작
    printf ("2\n");
    int dummy = 0;
    while (true) {
        printf ("3\n");
        if (conf_data.sync()) { 
            printf ("4\n");
            dnn.update (conf_data);
            printf ("5\n");
            if (WORK_FLAG == DONE_TAKE_PICTURE) { // 사진촬영을 모두 완료하였다면
                printf ("6\n");
                m.lock();
                WORK_FLAG = GO_INFERENCE;
                m.unlock();
                printf ("7\n");
                ups.upload_input (_io_data);
                printf ("8\n");
                dnn.inference(_io_data);
                printf ("9\n");
                ups.upload_output (_io_data);
                printf ("10\n");

                m.lock();
                WORK_FLAG = GO_TAKE_PICTURE; // 다시 사진촬영 요청
                m.unlock();
            }
            else {// 사진촬영중이므로 대기
                dummy++;
            }
        }
        else { // Admin Mode이므로 대기
            dummy++;
        }
    }
    return 0;
}