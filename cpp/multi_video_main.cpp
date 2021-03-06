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
    ConfigData confData;
    printf ("conf sync!\n");
    confData.sync();
    printf ("conf sync done!\n");
    OpenCV_DNN dnn (confData);
    printf ("dnn done !\n");
    Uploader ups;

    IOdata ioData (confData.cameraNumber);

    int MODE_FLAG = BASIC_MODE;
    int WORK_FLAG = GO_TAKE_PICTURE;

    std::vector<string> clntAddrs;
    bool allConnected = false;
    std::mutex m;
    std::thread camThr (cameraHandler, std::ref(ioData), std::ref(confData),
                        std::ref(clntAddrs), std::ref(allConnected),
                        std::ref(WORK_FLAG), std::ref(MODE_FLAG), std::ref(m));
    while (true)
        if (allConnected)
            break;

    while (true) {
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        m.lock()
        if (!confData.sync()) // prev==ADMIN && now==BASIC 이면 config 갱신
            dnn.update (confData);
        m.unlock()
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