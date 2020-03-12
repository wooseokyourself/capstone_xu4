#include web.hpp

Uploder::Uploder () {
    this->PHP_PATH = "src/web.php";
    this->WEBSERVER_PATH = "/home/html/ws/www/";
    this->UPLOAD_PATH = this->WEBSERVER_PATH + "uploads/";

    this->results = fopen (UPLOAD_PATH + "results.txt", "w");

    /* 웹서버의 디렉토리가 바뀌었다면 system 함수에 들어가는 경로 역시 바뀌어야함! */
    if (system ("cp web.php /home/html/ws/www/web.php")) {
        exit(-1);
    }
}

Uploader::~Uploader () {
    fclose (results);
}

Uploder::void
upload (Mat resultImg, int peopleNumber, string fileName) {
    /* UPLOAD_PATH/fileName 으로 이미지파일 저장 */
    imwrite (UPLOAD_PATH + fileName, resultImg);
    
    /* UPLOAD_PATH + "results.txt" 의 제일 밑줄에 "resultImg peopleNumber" 작성 */
    fputs (fileName + " " + peopleNumber + "\n", results);
}