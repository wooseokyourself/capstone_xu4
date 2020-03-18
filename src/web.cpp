#include "web.hpp"

Uploader::Uploader (string WEB_ROOT) {
    this->INPUT_IMG_SAVE_PATH = WEB_ROOT + "inputs/";
    this->OUTPUT_IMG_SAVE_PATH = WEB_ROOT + "outputs/";
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
    const char* info = (fileName + " " + "People: " + to_string(peopleNumber) + "\n").c_str();
    fputs (info, results);
	fclose (results);
}
