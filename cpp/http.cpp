#include "http.hpp"

namespace http {

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string
base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for(i = 0; (i < 4) ; i++)
            ret += base64_chars[char_array_4[i]];
        i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
        char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
        ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
        ret += '=';

    }

    return ret;

}

std::string
base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i ==4) {
        for (i = 0; i <4; i++)
            char_array_4[i] = base64_chars.find(char_array_4[i]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (i = 0; (i < 3); i++)
            ret += char_array_3[i];
        i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
        char_array_4[j] = 0;

        for (j = 0; j <4; j++)
        char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

void
postResultData (const string& url, const std::vector<cv::Mat>& imgs, const std::vector<int>& nums) {
    std::string currTime = getCurrTime()
    int imageNumber = imgs.size();
    
    // base64로 인코딩
    std::vector<string> encodeds (imgs.size());
    std::vector<uchar> bufs (imgs.size());
    for (int i=0; i<imageNumber; i++) {
        imencode(".jpeg", imgs[i], bufs[i]);
        auto *enc_msg = reinterpret_cast<unsigned char*>(bufs[i].data());
        encodeds[i] = base64_encode(enc_msg, bufs[i].size());
    }
    
    // json 데이터 구성
    Json::Value rootData;
    for (int i=0; i<imageNumber; i++) {
        int camID = i+1;
        Json::Value dataFromOneCam;
        dataFromOneCam["id"] = camID;
        dataFromOneCam["image"] = encodeds[i];
        dataFromOneCam["regDate"] = currTime;
        dataFromOneCam["peopleCNT"] = nums[i];
        rootData.append(dataFromOneCam);
    }

    // Json::Value 를 string 으로 변환
    Json::StyledWriter toString;
    std::string data = toString.write(rootData);
    
    RestClient::Response r;
    while(r.code != 200)
        RestClient::post(url, "application/json", data);
}

void
postImageForRoi (const string& url, const std::vector<cv::Mat>& imgs) {
    int imageNumber = imgs.size();

    // base64로 인코딩
    std::vector<string> encodeds (imgs.size());
    std::vector<uchar> bufs (imgs.size());
    for (int i=0; i<imageNumber; i++) {
        imencode(".jpeg", imgs[i], bufs[i]);
        auto *enc_msg = reinterpret_cast<unsigned char*>(bufs[i].data());
        encodeds[i] = base64_encode(enc_msg, bufs[i].size());
    }
    
    // json 데이터 구성
    Json::Value rootData;
    for (int i=0; i<imageNumber; i++) {
        int camID = i+1;
        Json::Value dataFromOneCam;
        dataFromOneCam["id"] = camID;
        dataFromOneCam["image"] = encodeds[i];
        rootData.append(dataFromOneCam);
    }

    // Json::Value 를 string 으로 변환
    Json::StyledWriter toString;
    std::string data = toString.write(rootData);
    
    RestClient::Response r;
    while(r.code != 200)
        RestClient::post(url, "application/json", data);
}

AdminData
getAdminSetting (const string& url) {
    RestClient::Response r;
    while(r.code != 200)
        RestClient::get(url);
    
    // r.body 의 데이터들을 data에 파싱
    AdminData data;
    Json::Value jsonData;
    Json::Reader reader;
    reader.parse(r.body, jsonData);
    data.captureResWidth = jsonData["sizeW"].asInt();
    data.captureResHeight = jsonData["sizeH"].asInt();
    data.resizeResWidth = jsonData["resizeW"].asInt();
    data.resizeResHeight = jsonData["resizeH"].asInt();
    data.cameraNumber = jsonData["camNum"].asInt();
    
    return data;
}

std::vector< vector<int> >
getRoiInfo (const string& url, const int& camNumber) {
    RestClient::Response r;
    while(r.code != 200)
        RestClient::get(url);
    
    // r.body 의 데이터들을 data에 파싱
    std::vector< vector<int> > data (camNumber);
    Json::Value jsonData;
    Json::Reader reader;
    reader.parse(r.body, jsonData);
    Json::ValueIterator it = jsonData.begin(); // 배열을 순회하기위한 이터레이터

    int camID = 1;
    while(it != jsonData.end()) {
        int camID = (*it)["id"].asInt();
        data[camID-1].push_back( (*it)["leftX"].asInt() );
        data[camID-1].push_back( (*it)["leftY"].asInt() );
        data[camID-1].push_back( (*it)["rightX"].asInt() );
        data[camID-1].push_back( (*it)["rightY"].asInt() );
    }

    return data;
}

}