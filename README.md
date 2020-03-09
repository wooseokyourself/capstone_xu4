#### 프로토콜 (인코딩된 이미지)
 std::vector<unsigned char>

---------------------------------------------------------------

#### make clean
> 모든 실행파일 제거

#### make delete
> 웹서버의 모든 실행파일 제거

#### make all
> 릴리즈버전, server.out

#### make debug
> #ifdef DEBUG
> 디버깅버전, server_debug.out

#### make debug_ml
> #ifdef DEBUG_ML
> 딥러닝테스트버전, server_debug_ml.out
> ml.hpp 및 ml.cpp의 코드를 통해 딥러닝 수행여부만을 테스트하기 위함.

###### 사용방법
1. 테스트할 .jpeg 이미지파일을 debug/test_images/ 디렉토리에 순차적인 숫자로 저장.
  (ex. 1.jpeg 2.jpeg 3.jpeg 4.jpeg)
2. 테스트할 이미지파일의 시작번호와 끝번호를 인자로 server_debug_ml.out 실행.
  (ex. ./server_debug_ml.out 1 4  -->  1.jpeg, 2.jpeg, 3.jpeg, 4.jpeg 파일에 대해 딥러닝 수행)
3. 결과는 웹서버의 uploads 폴더에서 확인.

--------------------------------------------------------------

#### 웹서버 루트
> /home/html/
