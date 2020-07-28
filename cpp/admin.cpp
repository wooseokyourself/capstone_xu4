#include "admin.hpp"

ConfigData::ConfigData () {
    this->prevFlag = ADMIN_MODE;
    this->confThreshold = 0.4;
    this->nmsThreshold = 0.5;

    this->readAdminInput();

    // overlaps가 입력되지 않을 시 초기값
    this->overlaps.resize(this->cameraNumber);
    for (int i=0; i<this->cameraNumber; i++) {
        this->overlaps[i].push_back(0);
        this->overlaps[i].push_back(0);
        this->overlaps[i].push_back(0);
        this->overlaps[i].push_back(0);
    }
}

/* Returns true when the server has to be reset with config data. */
bool
ConfigData::sync (bool isFirstCall) {
    int nowFlag = readModeFlag();
    ASSERT (nowFlag != -1);
    if (this->prevFlag == ADMIN_MODE && nowFlag == BASIC_MODE) {
        printf ("Configuration was changed. DNN has to be updated!\n");
        this->readAdminInput();
        if (!isFirstCall)
            this->readOverlaps();
        this->prevFlag = nowFlag;
        return true;
    }
    else if (this->prevFlag == ADMIN_MODE && nowFlag == ADMIN_MODE) {
        return false;
    }
    else if (this->prevFlag == BASIC_MODE && nowFlag == ADMIN_MODE) {
        this->prevFlag = nowFlag;
        return false;
    }
    else if (this->prevFlag == BASIC_MODE && nowFlag == BASIC_MODE) {
        return false;
    }
    else {
        return -1;
    }
}

int
ConfigData::readModeFlag () {
    char buf[20];
    const char* path = (CONFIG_PATH + "/mode.txt").c_str();
    FILE* fp = fopen (path, "r");
    ASSERT (fp != NULL);
    fgets (buf, sizeof(buf), fp);
    fclose (fp);
    string str (buf);
    if (str == "admin\n" || str == "admin")
        return ADMIN_MODE;
    else if (str == "basic\n" || str == "basic")
        return BASIC_MODE;
    else
        return -1;
}

void
ConfigData::readAdminInput () {
    /* 주어진 경로로부터 텍스트파일을 읽어서 모든 멤버변수에 저장하기 */
    char buf[20];
    FILE* fp;

    // Read admin_input.txt
    const char* path = (CONFIG_PATH + "/admin_input.txt").c_str();
    fp = fopen (path, "r");
    fgets(buf, sizeof(buf), fp);
    this->cameraNumber = atoi(buf);
    printf (" reading camera number=%d\n", this->cameraNumber);
    fgets(buf, sizeof(buf), fp);
    this->captureResWidth = atoi(buf);
    fgets(buf, sizeof(buf), fp);
    this->captureResHeight = atoi(buf);
    fgets(buf, sizeof(buf), fp);
    this->resizeResWidth = atoi(buf);
    fgets(buf, sizeof(buf), fp);
    this->resizeResHeight = atoi(buf);
    fclose (fp);
}

void
ConfigData::readOverlaps () {
    this->overlaps.clear();
    this->overlaps.resize(this->cameraNumber);

    char buf[20];
    FILE* fp;
    
    const char* path = (CONFIG_PATH + "/ROI.txt").c_str();
    fp = fopen (path, "r");
    for (int i=0; i<this->camera_number; i++) {
        fgets(buf, sizeof(buf), fp);
        char* ptr = strtok (buf, " ");
        while (ptr != NULL) {
            this->overlaps[i].push_back (atoi(ptr));
            ptr = strtok (NULL, " ");
        }
    }
    fclose (fp);

    // Admin Mode 에서 ROI를 재입력할 경우, overlaps의 제일 뒤에 0이 붙는 현상을 해결하기 위함
    for (int i=0; i<this->cameraNumber; i++)
        while ( (this->overlaps[i].size() % 4) != 0)
            this->overlaps[i].pop_back();

    printf ("read overlaps done!\n");
    for (int i=0; i<this->overlaps.size(); i++) {
        for (int j=0; j<this->overlaps[i].size(); j++) {
            printf ("%d ", overlaps[i][j]);
        }
        printf ("\n");
    }
}