#include "web.hpp"

Uploader::Uploader (string WEB_ROOT) {
    this->INPUT_IMG_SAVE_PATH = WEB_ROOT + "inputs/";
    this->OUTPUT_IMG_SAVE_PATH = WEB_ROOT + "outputs/";
}

void
Uploader::upload_input (Mat inputImg, string fileName = getCurrTime()) {
    imwrite (INPUT_IMG_SAVE_PATH + fileName, inputImg);
}

/*
    Write @resultMat as "@fileName.jpeg" into UPLOAD_PATH dir
    and renew 'results.txt'.
*/
void
Uploader::upload_output (Mat resultImg, int peopleNumber, string fileName = getCurrTime()) {
    imwrite (OUTPUT_IMG_SAVE_PATH + fileName, resultImg);

    // Open 'results.txt'.
	const char* results_PATH = (OUTPUT_IMG_SAVE_PATH + "results.txt").c_str();
	FILE* results = fopen (results_PATH, "a");	

    // Write "@resultImg People: @peopleNumber" in the bottom row of 'results.txt'.
    const char* info = (fileName + " " + "People: " + to_string(peopleNumber) + "\n").c_str();
    fputs (info, results);
	fclose (results);
}
