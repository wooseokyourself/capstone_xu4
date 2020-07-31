#include "socket.hpp"
#include "ml.hpp"
#include "uploader.hpp"
#include "admin.hpp"

string __root_path; // argv[1]

/* argv[1] <-- Root dir path of this project (absolute path) */
int
main (int argc, char* argv[]) {
    __root_path = string(argv[1]);
    ConfigData confData;
    confData.sync();
    OpenCV_DNN dnn (confData);
    Uploader ups;

    IOdata ioData (confData.cameraNumber);

    int MODE_FLAG = BASIC_MODE;
    int WORK_FLAG = GO_TAKE_PICTURE;

    // Camera connection
    std::vector<string> clntAddrs;
    bool allConnected = false;
    std::mutex m;
    std::thread camThr (cameraHandler, std::ref(ioData), std::ref(confData),
                        std::ref(clntAddrs), std::ref(allConnected),
                        std::ref(WORK_FLAG), std::ref(MODE_FLAG), std::ref(m)); // 소켓통신 시작
    while (true)
        if (allConnected)
            break;
    // Camera connection end


    // 카메라의 ip: clntAddrs[i]
    while (true) {
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        /* prev==ADMIN && now==BASIC 이면 config 갱신.
            즉, ADMIN mode인 동안에도 기존의 confData를 가지고 계속 BASIC task를 진행. */
        m.lock()
        if (!confData.sync())
            dnn.update (confData); // confData가 변경되었다면 dnn 설정 업데이트.
        m.unlock
        if (WORK_FLAG == DONE_TAKE_PICTURE) { // 사진촬영을 모두 완료하였다면
            printf (" WORK_FLAG: DONE_TAKE_PICTURE --> GO_INFERENCE\n");
            m.lock();
            WORK_FLAG = GO_INFERENCE;
            dnn.inference(ioData);
            http::postResultData("https://tproject-ye.herokuapp.com/api/basic/image-info", ioData.imgs, ioData.nums);
            ioData.clear();
            WORK_FLAG = GO_TAKE_PICTURE; // 다시 사진촬영 요청
            m.unlock();
            printf (" WORK_FLAG: DONE_TAKE_PICTURE --> GO_TAKE_PICTURE\n");
        }
    }
    return 0;
}