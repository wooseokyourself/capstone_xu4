#include "socket.hpp"

ssize_t
Recv (int sock, const void *buf, ssize_t size, ssize_t unit) {
printf ("Size: %d\n", size);
	ssize_t recvd = 0;
	ssize_t yet = size;
	while (recvd < size) {
		#ifdef DEBUG
		printf (" recvd: %d\n", recvd);
		#endif 
		if ( 0 < yet && yet < MAXBUFSIZE ){  /* 아직 받지않은 데이터가 MAXBUFSIZE보다 작은 경우 */
			printf ("걸려들었어! yet: %d\n", yet);
			printf ("recvd: %d", recvd);
			recvd += recv (sock, (void *) (buf + recvd/unit), yet, 0);
		}
		else if ( MAXBUFSIZE < yet )/* 받아야 할 데이터가 MAXBUFSIZE보다 큰 경우 */
			recvd += recv (sock, (void *) (buf + recvd/unit), MAXBUFSIZE, 0);
		else
			break;

		yet = size - recvd;
	}
	return recvd;
}

struct protocol*
RecvBuffer () {
    /*  LINGER 생성 */
    struct linger ling = {0, };
    ling.l_onoff = 1;	// linger use
    ling.l_linger = 0;	// linger timeout set

    /*  외부의 연결 요청을 처리하는 서버소켓 생성 */
    int servSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ASSERT (servSock != -1);
    
    /*  지역 주소 구조체 생성 */
    struct sockaddr_in servAddr;
    memset (&servAddr, 0, sizeof(servAddr));	    // 0으로 구조체 초기화
    servAddr.sin_family = AF_INET;		            // IPv4 주소 패밀리
    servAddr.sin_addr.s_addr = htonl (INADDR_ANY);  // 호스트의 어떤 IP로도 연결 요청 수락
    servAddr.sin_port = htons (PORT);	            // 지역포트
    
    /*  LINGER 설정: 서버소켓 */
    setsockopt (servSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));

    /*  지역 주소에 바인드 */
    ASSERT (bind (servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) >= 0);
    
    /*  servSock 이 들어오는 요청을 처리할 수 있도록 설정 */
    ASSERT (listen (servSock, MAXPENDING) >= 0);


    /*  클라이언트 소켓 */
    int clntSock;
    struct sockaddr_in clntAddr; // 클라이언트 주소 구조체 생성
    socklen_t clntAddrLen = sizeof(clntAddr);
    /*  LINGER 설정: 클라이언트소켓 */
    setsockopt (clntSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));

    /*  클라이언트의 연결을 기다림 */
	printf ("이제 연결을 기다립니다!\n");
    clntSock = accept (servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);


    // 이 시점에서 clntSock 이 실제 외부에서 연결을 요청한 클라이언트와 연결됨


    /*  연결된 클라이언트의 정보를 출력하는 기능 */
    char clntName[INET_ADDRSTRLEN]; // 클라이언트의 주소를 담기 위한 문자열
    if (inet_ntop (AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
	    printf ("Client connected %s%d\n", clntName, ntohs(clntAddr.sin_port));
    else
	    puts ("Unable to get client address");

	struct protocol* dataPtr = (struct protocol*) malloc (sizeof(struct protocol));
	int recvd;

	/*	data.buf.size() 받기 */
	ssize_t bufSize;
	#ifdef DEBUG
	printf ("data.buf.size() 받는중..\n");
	printf (" 받아야 할 버퍼사이즈: %d\n", sizeof(bufSize));
	#endif
	recvd = Recv (clntSock, &bufSize, sizeof(bufSize), sizeof(size_t *));

	/*	data.buf 받기 */
	#ifdef DEBUG
	printf ("data.buf 받는중..\n");
	printf (" 받아야 할 사이즈: %d\n", bufSize * sizeof(unsigned char));
	printf (" unit 사이즈: %d\n", sizeof(unsigned char));
	#endif
	
	std::vector<unsigned char> vec (bufSize*2);
	printf (" 할당된 vector 사이즈: %d\n", vec.size() * sizeof(unsigned char));
	recvd = Recv (clntSock, &vec[0], bufSize, sizeof(unsigned char));
	dataPtr->buf.assign (vec.begin(), vec.end());

	/*
	recvd = 0;
	unsigned char memValue;
	for (int i=0; i<bufSize; i++){
		recvd += recv (clntSock, (unsigned char*) &memValue, sizeof(unsigned char), 0);
		dataPtr->buf.push_back(memValue);
		printf ("[%d] set memValue: %d\n", i, dataPtr->buf[i]);
	}
	*/
	
	#ifdef DEBUG
	printf ("data.buf 받기 완료. 받은 사이즈: %d\n", recvd);
	#endif
	// ASSERT (recvd == dataPtr->buf.size() * sizeof(unsigned char));

	close (servSock);
    close (clntSock);

    return dataPtr;
}
