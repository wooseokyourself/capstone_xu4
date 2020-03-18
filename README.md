## 빌드환경
* Odroid XU4 (armv7l with no GPU)
* Ubuntu 16.04.6 LTS (Mate)
* OpenCV 4.1.2 with contrib (actually contrib is unnecessary)
* Apache 2.4.18
* PHP 7.0.33
* gcc/g++ 4.9.4
   
   
---------------------------------------------------------------
   
   
## 웹서버 구조
    /
> 출력된 이미지 및 사람 계수값을 시간 역순으로 출력 (index.php)   
> 현재 테스트머신에서 루트는 /home/html/ws/www/

    /inputs
> 입력된 이미지(.jpeg)를 저장하는 디렉토리

    /outputs
> 출력된 이미지(.jpeg) 및 results.txt를 저장하는 디렉토리

**프로그램 실행 시 루트 (/*) 초기화**
   
   
---------------------------------------------------------------
   
   
## 소켓통신 프로토콜
    std::vector<unsigned char>
> cv::Mat 이 인코딩된 자료형으로, 본 프로그램에서 다시 cv::Mat 으로 디코딩 진행
   
   
---------------------------------------------------------------
   
   
## 사용법

### Get YOLOv3 pre-trained model
    cd model
    ./getModels.sh
> coco.names 에서 Detect 하고 싶은 클래스만 남겨두고 나머지는 제거
   
   
### Compile

* make clean
> 모든 실행파일 제거

* make all
> 릴리즈버전, src/main.cpp

* make debug
> + 디버깅버전, src/debug_main.cpp, 소스코드에서 DEBUG 매크로 사용가능
> + 소켓통신 없이 **"로컬 이미지 입력 --> 딥러닝 출력 --> 입력 및 출력이미지 웹서버 업로드"** 만 진행   
> + 로컬 이미지 경로: debug/test_images/*.jpeg   
> + ./server_debug.out <처음: 테스트할 이미지 파일> <끝: 테스트할 이미지 파일>   
   
   
### Execution

* release
    1. ./debug.out
    2. 실행하면 std::vector<unsigned char> 자료형 수신대기

* debug
    1. 테스트할 .jpeg 이미지파일을 debug/test_images/ 디렉토리에 순차적인 숫자로 저장.   
        (ex. 1.jpeg 2.jpeg 3.jpeg 4.jpeg)
    2. 테스트할 이미지파일의 시작번호와 끝번호를 인자로 server_debug.out 실행.   
        (ex. ./server_debug.out 1 4  -->  1.jpeg, 2.jpeg, 3.jpeg, 4.jpeg 파일 입력)
    3. 결과는 웹서버의 uploads 폴더에서 확인.   

--------------------------------------------------------------
