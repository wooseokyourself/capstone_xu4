#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sendfile.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXLINE 511
#define MAXBUF 1024

static const int MAXPENDING = 5;    // 연결 요청을 대기할 수 있는 최대 수

void
error_handling (char *message) {
    fputs (message, stderr);
    printf("\n");
    exit(1);
}

int
main (int argc, char *argv[]) {

/* SERVER dafult set start */

    /* 명령어 인자의 개수 확인 */
    if (argc != 2) {
	    printf("Usage: %s <PORT>\n", argv[0]);
	    exit(1);
    }

    /* SET FOR SERVER SOCKET */
    
    /* 첫 번째 인자: 서버 포트번호 */
    in_port_t servPort = atoi (argv[1]);

    struct stat obj;
    
    /* 외부의 연결 요청을 처리하는 서버소켓 생성 */
    int servSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSock == -1)
	    error_handling ("socket() error");
    
    /* 지역 주소 구조체 생성 */
    struct sockaddr_in servAddr;
    memset (&servAddr, 0, sizeof(servAddr));	    // 0으로 구조체 초기화
    servAddr.sin_family = AF_INET;		    // IPv4 주소 패밀리
    servAddr.sin_addr.s_addr = htonl (INADDR_ANY);  // 호스트의 어떤 IP로도 연결 요청 수락
    servAddr.sin_port = htons (servPort);	    // 지역포트

    /* 필요 변수들 생성 */
    char buf[MAXBUF], fileName[MAXLINE];
    int k;
    int i;
    int fileSize; // 받고자 할 파일의 사이즈
    int c;
    int fileHandle;
    
    /* LINGER 생성 */
    struct linger ling = {0, };
    ling.l_onoff = 1;	// linger use
    ling.l_linger = 0;	// linger timeout set
    
    /* LINGER 설정: 서버소켓 */
    // setsockopt (servSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));

    /* 지역 주소에 바인드 */
    if (bind (servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
	error_handling ("bind() error");
    
    /* servSock 이 들어오는 요청을 처리할 수 있도록 설정 */
    if (listen (servSock, MAXPENDING) < 0)
	error_handling ("listen() error");

    /* SET FOR CLIENT SOCKET */

    /* 클라이언트 소켓 생성 */
    int clntSock;

    /* 클라이언트 주소 구조체 생성 */
    struct sockaddr_in clntAddr;
    socklen_t clntAddrLen = sizeof(clntAddr);

    /* LINGER 설정: 클라이언트소켓 */
    // setsockopt (clntSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));

    /* 클라이언트의 연결을 기다림 */
    clntSock = accept (servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);

/* SERVER dafult set end */

    /* 이 시점에서 clntSock 이 실제 외부에서 연결을 요청한 클라이언트와 연결됨 */

    /* 연결된 클라이언트의 정보를 출력하는 기능 */
    char clntName[INET_ADDRSTRLEN]; // 클라이언트의 주소를 담기 위한 문자열
    if (inet_ntop (AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
	printf ("Client connectec %s%d\n", clntName, ntohs(clntAddr.sin_port));
    else
	puts ("Unable to get client address");


	/*  
            from: send(sock, buf, 100, 0);
            put 혹은 quit 이 buf 로 들어옴
        */
    memset (buf, 0x00, MAXBUF);
    recv (clntSock, buf, 100, 0);
    
    sscanf (buf, "%s", fileName); // fileName 저장

            /* 
                from: send(sock, &imgSize, sizeof(int), 0); 
                &imgSize 를 &fileSize 로 받음
            */
    recv (clntSock, &fileSize, sizeof(int), 0);
    printf ("fileSize: %d\n", fileSize);

    /*  fileSize, fileName 를 토대로 파일 생성 */
    while (1) { 
        fileHandle = open (fileName, O_CREAT | O_EXCL | O_WRONLY, 0666);
        if (fileHandle == -1) // 만약 동일한파일이 서버에 존재한다면
            sprintf (fileName + strlen(fileName), "_1"); // 이름 뒤에 _1 붙이기
        else // 파일을 정상적으로 생성할 수 있다면
            break;
        }

    /*
        from: sendfile(sock, filehandle, NULL, imgSize); 
        파일 수신
    */
    int file_read_len, written_len;
	    
    while ( (file_read_len = recv (clntSock, buf, MAXBUF, 0)) > 0) {
	written_len = write (fileHandle, buf, file_read_len);
	if (written_len != file_read_len) {
	    error_handling ("written_len != file_read_len");
	}
	memset (buf, 0x00, MAXBUF);
    }

    close (fileHandle);
    close (clntSock);
    
    return 0;
}
