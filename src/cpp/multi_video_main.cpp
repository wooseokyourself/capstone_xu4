#include "from_multi_video.hpp"
#include "ml.hpp"
#include "uploader.hpp"
#include "admin.hpp"

string __root_path;

/* 어플 최초 실행시 conf_data를 웹에서 입력받은 뒤에 이 C++ 프로그램이 실행되어야 config data를 초기화할 수 있음 */
int
main (int argc, char* argv[]) {
    __root_path = string(argv[1]);
    printf ("start program\n");
    config_data _conf_data;
    printf ("conf sync!\n");
    // conf_data.sync(); // 서비스 최초 실행시 제일 먼저 conf_data를 읽어온다.
    _conf_data.sync();
    printf ("conf sync done!\n");
    OpenCV_DNN dnn (_conf_data);
    printf ("dnn done !\n");
    Uploader ups;

    io_data _io_data (_conf_data.camera_number);

    int MODE_FLAG = BASIC_MODE;
    int WORK_FLAG = GO_TAKE_PICTURE;

    std::vector<string> clnt_addrs;
    bool allConnected = false;
    std::mutex m;
    std::thread cam_thr (camera_handler, std::ref(_io_data), std::ref(_conf_data),
                        std::ref(clnt_addrs), std::ref(allConnected),
                        std::ref(WORK_FLAG), std::ref(MODE_FLAG), std::ref(m));
    int dummy = 0;
    while (true) {
        if (allConnected)
            break;
        else
            dummy++;
    }

    ups.upload_ips (clnt_addrs);

    while (true) {
        if (_conf_data.sync()) // prev==ADMIN && now==BASIC 이면 config 갱신
            dnn.update (_conf_data);
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
        else
            dummy++;
    }
    return 0;
}