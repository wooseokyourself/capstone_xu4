#include "web.hpp"

Uploader::Uploader () {
    this->PHP_PATH = "src/web.php";
    this->WEBSERVER_PATH = "/home/html/ws/www/";
    this->UPLOAD_PATH = this->WEBSERVER_PATH + "uploads/";
    const char* results_PATH = (UPLOAD_PATH + "results.txt").c_str();
    this->results = fopen (results_PATH, "w");

    /* 웹서버의 디렉토리가 바뀌었다면 system 함수에 들어가는 경로 역시 바뀌어야함! */
    if (system ("cp web.php /home/html/ws/www/web.php")) {
        exit(-1);
    }
}

Uploader::~Uploader () {
    fclose (results);
}

/* resultMat 이미지를 UPLOAD_PATH/fileName 에 쓰고, UPLOAD_PATH/results.txt 작성 */
void
Uploader::upload (Mat resultImg, int peopleNumber, string fileName) {
    /* UPLOAD_PATH/fileName 으로 이미지파일 저장 */
    imwrite (UPLOAD_PATH + fileName, resultImg);
    
    /* UPLOAD_PATH + "results.txt" 의 제일 밑줄에 "resultImg peopleNumber" 작성 */
    const char* info = (fileName + " " + peopleNumber + "\n").c_str();
    fputs (info, results);
}