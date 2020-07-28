#include "uploader.hpp"

void
Uploader::uploadIPs (const vector<string>& clntAddrs) {
    const char* path = (RESULT_DIR_PATH + "/camera_ip.txt").c_str();
    FILE* fp = fopen (path, "a");
    for (int i=0; i<clntAddrs.size(); i++) {
        const char* eachClntName = (clntAddrs[i] + "\n").c_str();
        fputs (eachClntName, fp);
    }
    fclose (fp);
}

void
Uploader::uploadInput (const IOdata& ioData) {
    for (int i=0; i<ioData.imgs.size(); i++) {
        imwrite (ROI_DIR_PATH + "/" + to_string(i+1) + ".jpeg", ioData.imgs[i]);
    }
}

/*
    Write @resultMat as "@fileName.jpeg" into UPLOAD_PATH dir
    and renew 'results.txt'.
*/
void
Uploader::uploadOutput (const IOdata& ioData) {
    string fileName = getCurrTime();

    for (int i=0; i<ioData.imgs.size(); i++) {
        imwrite (RESULT_DIR_PATH + "/images/" + to_string(i+1) + "/" + fileName + "_" + to_string(i+1) + ".jpeg", ioData.imgs[i]);
    }

    // Open 'people.txt'.
	const char* resultsPATH = (RESULT_DIR_PATH + "/people.txt").c_str();
	FILE* fp = fopen (resultsPATH, "a");	

    /*
    const char* totalNum = (to_string(ioData.totalPeopleNum) + "\n").c_str();
    fputs (totalNum, fp);
    */
   
    for (int i=0; i<ioData.imgs.size(); i++) {
        const char* eachPictureName = (fileName + "_" + to_string(i+1) + ".jpeg" + "\n").c_str();
        const char* eachPictureNum = (to_string(ioData.nums[i]) + "\n").c_str();
        fputs (eachPictureName, fp);
        fputs (eachPictureNum, fp);
    }
	fclose (fp);
}
