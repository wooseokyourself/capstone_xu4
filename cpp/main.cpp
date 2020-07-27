#include "socket.hpp"
#include "ml.hpp"
#include "uploader.hpp"
#include "admin.hpp"

string __root_path; // argv[1]

/* argv[1] <-- Root dir path of this project (absolute path) */
int
main (int argc, char* argv[]) {
    __root_path = string(argv[1]);
    config_data _conf_data;
    _conf_data.sync(true);
    OpenCV_DNN dnn (_conf_data);
    Uploader ups;

    io_data _io_data (_conf_data.camera_number);

    int MODE_FLAG = BASIC_MODE;
    int WORK_FLAG = GO_TAKE_PICTURE;

    // Camera connection
    std::vector<string> clnt_addrs;
    bool allConnected = false;
    std::mutex m;
    std::thread cam_thr (camera_handler, std::ref(_io_data), std::ref(_conf_data),
                        std::ref(clnt_addrs), std::ref(allConnected),
                        std::ref(WORK_FLAG), std::ref(MODE_FLAG), std::ref(m)); // 소켓통신 시작
    while (true)
        if (allConnected)
            break;
    ups.upload_ips (clnt_addrs);
    // Camera connection end

    while (true) {
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        /* prev==ADMIN && now==BASIC 이면 config 갱신.
            즉, ADMIN mode인 동안에도 기존의 conf_data를 가지고 계속 BASIC task를 진행. */
        if (_conf_data.sync(false))
            dnn.update (_conf_data); // _conf_data가 변경되었다면 dnn 설정 업데이트.
        if (WORK_FLAG == DONE_TAKE_PICTURE) { // 사진촬영을 모두 완료하였다면
            printf (" WORK_FLAG: DONE_TAKE_PICTURE --> GO_INFERENCE\n");
            m.lock();
            WORK_FLAG = GO_INFERENCE;
            ups.upload_input (_io_data);
            dnn.inference(_io_data);
            ups.upload_output (_io_data);
            _io_data.clear();
            WORK_FLAG = GO_TAKE_PICTURE; // 다시 사진촬영 요청
            m.unlock();
            printf (" WORK_FLAG: DONE_TAKE_PICTURE --> GO_TAKE_PICTURE\n");
        }
    }
    return 0;
}