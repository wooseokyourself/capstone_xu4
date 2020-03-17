#ifndef WEB
#define WEB

#include "common.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;
using namespace std;

/*
    프로그램이 실행되면,
    1. 최초에 src 디렉토리에 있는 모든 .php 파일을 /home/html/ws/www 로 복사하기. (Uploader 생성자)
    2. 딥러닝 결과가 나올 때마다 /home/html/ws/www/uploads 에 옮기기.
        - .jpeg 파일은 그냥 쓰기, 사람의 수는 results.txt 파일에 한 줄당 ".jpeg 파일명" "people number" 양식으로 기입하기.
    3. 웹브라우저에서 웹서버로 접속할 때마다 web.php가 실행되는 상황에서 일어나는 일은...
        3.1 www/uploads 디렉토리에 있는 .jpeg 파일의 목록을 가져온다.
        3.2 www/uploads/results.txt 를 읽는다.
        3.3 읽어들인 파일 값과 results.txt의 값을 토대로 웹페이지에 출력.
*/

class Uploader {
public:
    Uploader();

    void
    upload_input (Mat inputImg, string fileName);

    void
    upload_output (Mat resultImg, int peopleNumber, string fileName);

private:
    string INDEX_PATH;
    string WEBSERVER_PATH;
    string INPUT_IMG_SAVE_PATH;
    string OUTPUT_IMG_SAVE_PATH;
    char* results_PATH;
};

#endif
