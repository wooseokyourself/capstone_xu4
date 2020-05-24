#include "web.hpp"

Uploader::Uploader (string ROI_DIR_PATH, string RESULT_DIR_PATH) {
    this->ROI_DIR_PATH = ROI_DIR_PATH;
    this->RESULT_DIR_PATH = RESULT_DIR_PATH;
}

void
Uploader::upload_input (const io_data& _io_data) {
    for (int i=0; i<_io_data.imgs.size(); i++) {
        imwrite (ROI_DIR_PATH + (i+1) + ".jpeg", _io_data.imgs[i]);
    }
}

/*
    Write @resultMat as "@file_name.jpeg" into UPLOAD_PATH dir
    and renew 'results.txt'.
*/
void
Uploader::upload_output (const io_data& _io_data, string file_name = getCurrTime()) {
    for (int i=0; i<_io_data.imgs.size(); i++) {
        imwrite (RESULT_DIR_PATH + (i+1) + "/" + file_name + "_" + (i+1) + ".jpeg", _io_data.imgs[i]);
    }

    // Open 'people.txt'.
	const char* results_PATH = (RESULT_DIR_PATH + "people.txt").c_str();
	FILE* fp = fopen (results_PATH, "a");	

    const char* total_num = (_io_data.total_people_num + "\n").c_str();
    fputs (total_num, fp);

    for (int i=0; i<_io_data.imgs.size(); i++) {
        const char* each_picture_name = (file_name + "_" + (i+1) + ".jpeg" + "\n").c_str();
        const char* each_picture_num = (_io_data.nums[i] + "\n").c_str();
        fputs (each_picture_name, fp);
        fputs (each_picture_num, fp);
    }
	fclose (results);
}
