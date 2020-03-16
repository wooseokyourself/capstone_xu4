#include "web.hpp"

Uploader::Uploader () {
    this->PHP_PATH = "src/web.php";
    this->WEBSERVER_PATH = "/home/html/ws/www/";
    this->UPLOAD_PATH = this->WEBSERVER_PATH + "uploads/";

    /* 웹서버의 디렉토리가 바뀌었다면 system 함수에 들어가는 경로 역시 바뀌어야함! */

	/* 웹서버 web.php 초기화 */
    system ("cp src/web.php /home/html/ws/www/web.php");

	/* 웹서버 uploads/ 초기화 */
	system ("rm -rf /home/html/ws/www/uploads/*");
	system ("touch /home/html/ws/www/uploads/results.txt");
}

/* resultMat 이미지를 UPLOAD_PATH/fileName 에 쓰고, UPLOAD_PATH/results.txt 작성 */
void
Uploader::upload (Mat resultImg, int peopleNumber, string fileName) {
	/* results.txt 열기 */
	const char* results_PATH = (UPLOAD_PATH + "results.txt").c_str();
	FILE* results = fopen (results_PATH, "a");	

    /* UPLOAD_PATH/fileName 으로 이미지파일 저장 */
    imwrite (UPLOAD_PATH + fileName, resultImg);
    
    /* UPLOAD_PATH + "results.txt" 의 제일 밑줄에 "resultImg peopleNumber" 작성 */
    const char* info = (fileName + " " + to_string(peopleNumber) + "\n").c_str();
    fputs (info, results);
	fclose (results);
}
