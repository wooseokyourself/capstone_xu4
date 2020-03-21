## Test Environment and Dependencies
* Odroid XU4 (armv7l, there is also GPU, but not used)
* Ubuntu 16.04.6 LTS (Mate)
* gcc/g++ 4.9.4
* Makefile
* OpenCV 4.1.2 with contrib (actually contrib is unnecessary)
* Apache 2.4.18
* PHP 7.0.33
   
   
---------------------------------------------------------------
   
   
## Web Structures
    $(WEB_ROOT)/
> 출력된 이미지 및 사람 계수값을 시간 역순으로 출력 (index.php)   

    $(WEB_ROOT)/inputs
> 입력된 이미지(.jpeg)를 저장하는 디렉토리

    $(WEB_ROOT)/outputs
> 출력된 이미지(.jpeg) 및 results.txt를 저장하는 디렉토리
   
   
---------------------------------------------------------------
   
   
## Application Protocol
    std::vector<unsigned char>
> cv::Mat 이 인코딩된 자료형으로, 본 프로그램에서 다시 cv::Mat 으로 디코딩 진행
   
   
---------------------------------------------------------------
   
   
## Install

### 1. Get YOLOv3 pre-trained model
    cd model
    ./getModels.sh
> coco.names 에서 person 클래스만 남겨두고 나머지는 제거

   
### 2. Set root path of your web into $(WEB_ROOT) in Makefile
> The path **MUST be ABSOLUTE path**.
> Also there must be **'/'** in the end of your path.   
   ex. WEB_ROOT = /home/html/ws/www/
   
   
### 3. Compile

#### 3.1. Compile Release ver.   
    make all

#### 3.2. Compile Debugging ver.   
    make debug
> 소켓통신 없이 **"로컬 이미지 입력 --> 딥러닝 출력 --> 입력 및 출력이미지 웹서버 업로드"** 만 진행
   
   
---------------------------------------------------------------
   
   
## Usage

### 1. Init root dir of your Web   
    make init
   
### 2. Run

#### 2.1. Run Release ver.   
    make run   
> + then the program stuck in listen() to wait connection request.   
> + also see https://github.com/wooseokyourself/capstone_pi
   
#### 2.2. Run Debugging ver.   
    make debug_run
> + Program reads **.jpeg** files from '*debug/images/*' sequentially from 1.   
>     (ex. debug/images/*1.jpeg*, debug/images/*2.jpeg*, ... debug/images/**$(last).jpeg**)
> + 'make debug_run' will ask you the value of $(last)

---------------------------------------------------------------
   
   
## Others

### Removes *.out
    make clean


--------------------------------------------------------------
   
   
## Example Result of Web (index.php)

### from Raspi camera (Release ver.)
![시연2](https://user-images.githubusercontent.com/49421142/76927049-53dc3880-6921-11ea-9b9f-eae576d58d72.JPG)

### from test image (Debugging ver.)
![캡처](https://user-images.githubusercontent.com/49421142/76976201-fecb1180-6976-11ea-81db-f5a9195ffcca.JPG)

