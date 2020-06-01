#include "admin.hpp"

config_data::config_data () {
    this->prev_flag = ADMIN_MODE;
    this->confThreshold = 0.4;
    this->nmsThreshold = 0.5;

    this->read_admin_input();

    // ovlaps가 입력되지 않을 시 초기값
    this->ovlaps.resize(this->camera_number);
    for (int i=0; i<this->camera_number; i++) {
        this->ovlaps[i].push_back(0);
        this->ovlaps[i].push_back(0);
        this->ovlaps[i].push_back(0);
        this->ovlaps[i].push_back(0);
    }
}

/* Returns true when the server has to be reset with config data. */
bool
config_data::sync (bool is_first_call) {
    int now_flag = read_mode_flag();
    ASSERT (now_flag != -1);
    if (prev_flag == ADMIN_MODE && now_flag == BASIC_MODE) {
        // printf ("prev_flag == ADMIN && now_flag == BASIC\n");
        this->read_admin_input();
        if (!is_first_call)
            this->read_ovlaps();
        prev_flag = now_flag;
        return true;
    }
    else if (prev_flag == ADMIN_MODE && now_flag == ADMIN_MODE) {
        // printf ("prev_flag == ADMIN && now_flag == ADMIN\n");
        return false;        
    }
    else if (prev_flag == BASIC_MODE && now_flag == ADMIN_MODE) {
        // printf ("prev_flag == BASIC && now_flag == ADMIN\n");
        prev_flag = now_flag;
        return false;
    }
    else if (prev_flag == BASIC_MODE && now_flag == BASIC_MODE) {
        // printf ("prev_flag == BASIC && now_flag == BASIC\n");
        return false;
    }
    else {
        return -1;
    }
}

int
config_data::read_mode_flag () {
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
config_data::read_admin_input () {
    /* 주어진 경로로부터 텍스트파일을 읽어서 모든 멤버변수에 저장하기 */
    char buf[20];
    FILE* fp;

    // Read admin_input.txt
    const char* path = (CONFIG_PATH + "/admin_input.txt").c_str();
    fp = fopen (path, "r");
    fgets(buf, sizeof(buf), fp);
    this->camera_number = atoi(buf);
    printf (" reading camera number=%d\n", this->camera_number);
    fgets(buf, sizeof(buf), fp);
    this->capture_res_width = atoi(buf);
    fgets(buf, sizeof(buf), fp);
    this->capture_res_height = atoi(buf);
    fgets(buf, sizeof(buf), fp);
    this->resize_res_width = atoi(buf);
    fgets(buf, sizeof(buf), fp);
    this->resize_res_height = atoi(buf);
    fclose (fp);

    // ROI.txt 파일 여기에서 읽는 기능 추가해야함
}

void
config_data::read_ovlaps () {
    this->ovlaps.clear();
    this->ovlaps.resize(this->camera_number);

    char buf[20];
    FILE* fp;
    
    const char* path = (CONFIG_PATH + "/ROI.txt").c_str();
    fp = fopen (path, "r");
    for (int i=0; i<this->camera_number; i++) {
        fgets(buf, sizeof(buf), fp);
        char* ptr = strtok (buf, " ");
        while (ptr != NULL) {
            this->ovlaps[i].push_back (atoi(ptr));
            ptr = strtok (NULL, " ");
        }
    }
    fclose (fp);

    printf ("read ovlaps done!\n");
    for (int i=0; i<this->ovlaps.size(); i++) {
        for (int j=0; j<this->ovlaps[i].size(); j++) {
            printf ("%d ", ovlaps[i][j]);
        }
        printf ("\n");
    }
}