#include "socket.hpp"
#include "ml.hpp"
#include "web.hpp"
#include "admin.hpp"

#define CONFIG_PATH $(ROOT)/path/blah/
#define ROI_DIR_PATH
#define RESULT_DIR_PATH $(ROOT)/blah/blah/

/* 어플 최초 실행시 conf_data를 웹에서 입력받은 뒤에 이 C++ 프로그램이 실행되어야 config data를 초기화할 수 있음 */
int
main (int argc, char* argv[]) {
    config_data conf_data (CONFIG_PATH);
    conf_data.read(); // 서비스 최초 실행시 제일 먼저 conf_data를 읽어온다.
    OpenCV_DNN dnn (conf_data.resize_res, conf_data.confThreshold, conf_data.nmsThreshold);
    Uploader ups (RESULT_DIR_PATH);

    io_data _io_data (conf_data.camera_number);

    int MODE_FLAG = BASIC_MODE;
    int WORK_FLAG = GO_TAKE_PICTURE;

    std::mutex m;
    std::thread cam_thr (camera_handler, std::ref(_io_data.imgs), std::ref(conf_data.camera_number), 
                        std::ref(WORK_FLAG), std::ref(MODE_FLAG), std::ref(m)); // 소켓통신 시작
    
    int dummy = 0;
    while (true) {
        if (MODE_FLAG == ADMIN_MODE) {
            conf_data.read();
            dnn.update (conf_data);
            MODE_FLAG == BASIC_MODE;
        }
        else if (MODE_FLAG == BASIC_MODE) {
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
        else { // terminate
            break;
        }
    }
    return 0;
}