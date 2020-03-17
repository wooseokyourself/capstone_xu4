#include "web.hpp"

Uploader::Uploader () {
    this->INDEX_PATH = "src/web/1-basic.php";
    this->WEBSERVER_PATH = "/home/html/ws/www/";
    this->INPUT_IMG_SAVE_PATH = this->WEBSERVER_PATH + "inputs/";
    this->OUTPUT_IMG_SAVE_PATH = this->WEBSERVER_PATH + "outputs/";

    /* 웹서버의 디렉토리가 바뀌었다면 system 함수에 들어가는 경로 역시 바뀌어야함! */

	/* 웹서버 초기화 */
    system ("cp src/web/* /home/html/ws/www/");
    // system ("rm -rf /home/html/ws/www/inputs/*");
	// system ("rm -rf /home/html/ws/www/outputs/*");
	system ("touch /home/html/ws/www/outputs/results.txt");
}

void
Uploader::upload_input (Mat inputImg, string fileName = getCurrTime()) {
    imwrite (INPUT_IMG_SAVE_PATH + fileName, inputImg);
}

/* resultMat 이미지를 UPLOAD_PATH/fileName 에 쓰고, UPLOAD_PATH/results.txt 작성 */
void
Uploader::upload_output (Mat resultImg, int peopleNumber, string fileName = getCurrTime()) {
    imwrite (OUTPUT_IMG_SAVE_PATH + fileName, resultImg);

    /* results.txt 열기 */
	const char* results_PATH = (OUTPUT_IMG_SAVE_PATH + "results.txt").c_str();
	FILE* results = fopen (results_PATH, "a");	

    /* UPLOAD_PATH + "results.txt" 의 제일 밑줄에 "resultImg peopleNumber" 작성 */
    const char* info = (fileName + " " + to_string(peopleNumber) + "\n").c_str();
    fputs (info, results);
	fclose (results);
}
