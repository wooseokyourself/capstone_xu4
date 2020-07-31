#ifndef HTTP
#define HTTP

#include "common.hpp"
#include "admin.hpp"
#include <restclient-cpp/restclient.h>
#include <json/json.h>

using namespace http;

namespace http {

std::string
base64_encode(unsigned char const* , unsigned int len);

std::string
base64_decode(std::string const& s);

void
postResultData (const string& url, const std::vector<cv::Mat>& imgs, const std::vector<int>& nums);

void
postImageForRoi (const string& url, const std::vector<cv::Mat>& imgs);

AdminData
getAdminSetting (const string& url);

std::vector< vector<int> >
getRoiInfo (const string& url, const int& camNumber);

}



#endif