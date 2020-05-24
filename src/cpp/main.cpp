#include "test.hpp"
#include "ml.hpp"
#include "uploader.hpp"
#include "admin.hpp"

/* 어플 최초 실행시 conf_data를 웹에서 입력받은 뒤에 이 C++ 프로그램이 실행되어야 config data를 초기화할 수 있음 */
int
main (int argc, char* argv[]) {
    config_data conf_data ();
    conf_data.sync(); // 서비스 최초 실행시 제일 먼저 conf_data를 읽어온다.
    OpenCV_DNN dnn (conf_data.resize_res, conf_data.confThreshold, conf_data.nmsThreshold);
    Uploader ups (conf_data.camera_number);

    io_data _io_data (conf_data.camera_number);

    int MODE_FLAG = BASIC_MODE;
    int WORK_FLAG = GO_TAKE_PICTURE;

    std::mutex m;
    std::thread cam_thr (camera_handler, std::ref(_io_data.imgs), std::ref(conf_data.camera_number), 
                        std::ref(WORK_FLAG), std::ref(MODE_FLAG), std::ref(m)); // 소켓통신 시작
    
    int dummy = 0;
    while (true) {
        if (conf_data.sync()) { 
            dnn.update (conf_data);
            if (WORK_FLAG == DONE_TAKE_PICTURE) { // 사진촬영을 모두 완료하였다면
                m.lock();
                WORK_FLAG = GO_INFERENCE;
                m.unlock();

                ups.upload_input (_io_data);
                dnn.inference(_io_data);
                ups.upload_output (_io_data);

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