#include "common.hpp"

extern string __root_path;

string
_ROOT__PATH() { return __root_path; }

string
_CONFIG__PATH() { return (_ROOT__PATH() + "/config"); }

string
_BIN__PATH() { return (_ROOT__PATH() + "/bin"); }

string
_ROI__DIR__PATH() { return (_CONFIG__PATH() + "/images"); }

string
_RESULT__DIR__PATH() { return (_ROOT__PATH() + "/resources"); }

/* Return "YYYY_MM_DD_HH:MM:SS" */
string
getCurrTime () {
    time_t timeObj = time(NULL);
	struct  tm* tm = localtime(&timeObj);
    std::string currTime, temp;
	currTime += std::to_string (tm->tm_year+1900);
    temp = std::to_string (tm->tm_mon+1);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
    temp = std::to_string (tm->tm_mday);
    if (temp.length() == 1) temp = "0" + temp;
    currTime += temp;
	temp = std::to_string (tm->tm_hour);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
	temp = std::to_string (tm->tm_min);
	if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
	temp = std::to_string (tm->tm_sec);
    if (temp.length() == 1) temp = "0" + temp;
	currTime += temp;
    return currTime;
}

io_data::io_data (int camera_num) {
    this->imgs.resize (camera_num);
    this->nums.resize (camera_num);
    this->total_people_num = 0;
    this->inference_time = 0;
}

io_data::~io_data () {
    this->imgs.clear();
    this->nums.clear();
}