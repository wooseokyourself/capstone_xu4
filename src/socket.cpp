#include "socket.hpp"

ssize_t
Recv (int sock, const void *buf, ssize_t size, ssize_t unit) {
	ssize_t recvd = 0;
	ssize_t yet = size;
	while (recvd < size) {
		if ( 0 < yet && yet < MAXBUFSIZE) {
			recvd += recv (sock, (void *) (buf + recvd/unit), yet, 0);
			yet = size - recvd;
		}
		else {
			recvd += recv (sock, (void *) (buf + recvd/unit), MAXBUFSIZE, 0);
			yet = size - recvd;
		}
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
    clntSock = accept (servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);


    // 이 시점에서 clntSock 이 실제 외부에서 연결을 요청한 클라이언트와 연결됨


    /*  연결된 클라이언트의 정보를 출력하는 기능 */
    char clntName[INET_ADDRSTRLEN]; // 클라이언트의 주소를 담기 위한 문자열
    if (inet_ntop (AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
	    printf ("Client connected %s%d\n", clntName, ntohs(clntAddr.sin_port));
    else
	    puts ("Unable to get client address");

	int recvd;
	int yet;
	struct protocol* dataPtr = (struct protocol*) malloc (sizeof(struct protocol));
	
	/*	Recv data.prevBuf.size() */
	ssize_t prevBufSize;
	Recv (clntSock, &prevBufSize, sizeof(prevBufSize), sizeof(size_t *));
	dataPtr->prevBuf.resize(prevBufSize);

	/*	Recv data.prevBuf */
	Recv (clntSock, &(*dataPtr->prevBuf.begin()), prevBufSize, sizeof(unsigned char));

	/*	Recv data.currBuf.size() */
	ssize_t currBufSize;
	Recv (clntSock, &currBufSize, sizeof(currBufSize), sizeof(size_t *));
	dataPtr->currBuf.resize(currBufSize);

	/*	Recv data.currBuf */
	Recv (clntSock, &(*dataPtr->currBuf.begin()), currBufSize, sizeof(unsigned char));

	/*	Recv data.diffValue */
	Recv (clntSock, &dataPtr->diffValue, sizeof(dataPtr->diffValue), sizeof(uint32_t *));

	close (servSock);
    close (clntSock);
	
#ifdef DEBUG
	printf ("\nData is like..\n\n");
	printf ("@@prevImg begin\n");
	for (int i=0; i<20; i++) 
		printf (" %d", dataPtr->prevBuf[i]);
	printf ("\n@@ ... and end\n");
	for (int i=dataPtr->prevBuf.size()-1; i>dataPtr->prevBuf.size()-21; i--)
		printf (" %d", dataPtr->prevBuf[i]);
	printf ("\n\n");
	printf ("@@currImg begin\n");
	for (int i=0; i<20; i++) 
		printf (" %d", dataPtr->currBuf[i]);
	printf ("\n@@ ... and end\n");
	for (int i=dataPtr->currBuf.size()-1; i>dataPtr->currBuf.size()-21; i--)
		printf (" %d", dataPtr->currBuf[i]);
	printf ("\n\n");
#endif

    return dataPtr;
}
