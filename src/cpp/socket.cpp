#include "socket.hpp"

ssize_t
Recv (const int& sock, const void *buf, ssize_t size, ssize_t unit) {
	ssize_t recvd = 0;
	ssize_t yet = size;
	while (recvd < size) {
		if ( 0 < yet && yet < MAXBUFSIZE )  /* 아직 받지않은 데이터가 MAXBUFSIZE보다 작은 경우 */
			recvd += recv (sock, (void *) (buf + recvd/unit), yet, 0);
		else /* 받아야 할 데이터가 MAXBUFSIZE보다 큰 경우 */
			recvd += recv (sock, (void *) (buf + recvd/unit), MAXBUFSIZE, 0);

		yet = size - recvd;
	}
	return recvd;
}

/* Notify the client to take a picture. */
void
send_notification (const int& clntSock) {
    printf ("Third, send notification\n");
    printf (" >> notification's size: %d\n", sizeof(bool));
    bool notification = true;
    int sent = send (clntSock, &notification, sizeof(notification), 0);
    printf (" >> sent: %d\n", sent);
    ASSERT (sent == sizeof(notification));
}

void
send_terminate_flag (const int& clntSock, int& MODE_FLAG) {
    printf ("Second, send terminate_flag\n");
    bool terminate_flag;
    if (MODE_FLAG == TERMINATE_MODE)
        terminate_flag = true;
    else
        terminate_flag = false;
    printf (" >> terminate flag's size: %d\n", sizeof(terminate_flag));
    int sent = send (clntSock, &terminate_flag, sizeof(terminate_flag), 0);
    printf (" >> sent: %d\n", sent);
    ASSERT (sent == sizeof(terminate_flag));
}

/* Receive pictures in each thread. */
void
handle_thread (const int& clntSock, std::vector<cv::Mat>& imgs, bool& picture_flag, int& MODE_FLAG, std::mutex& m) {
    int dummy;
    int recvd;

    // Receive id of cam
    printf ("First, recv a camId...\n");
    int camId;
    recvd = Recv (clntSock, &camId, sizeof(camId), 1);
    ASSERT (recvd == sizeof(camId));
    printf (" >> Got camId: %d\n", camId);
    while (true) { // 베이직 모드라면 이 스레드 계속 실행
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        if (!picture_flag) { // 사진을 가져오라는 명령이 떨어짐
            // 스레드별로 camId를 먼저 수신
            // 이후 데이터를 수신하고 디코딩하여 imgs[camId-1] 에 저장.
            // imgs[camId-1] 저장이 끝난 스레드는 종료.

            send_terminate_flag (clntSock, MODE_FLAG);

            // Send notification
            send_notification (clntSock);

            // Receive @vec.size()
            recvd = 0;
            ssize_t bufSize;
            recvd = Recv (clntSock, &bufSize, sizeof(bufSize), sizeof(size_t *));

            // Receive @vec
            vector<unsigned char> vec(bufSize);
            recvd = Recv (clntSock, &vec[0], bufSize, sizeof(unsigned char));
            ASSERT (recvd == vec.size() * sizeof(unsigned char));

            imgs[camId-1] = cv::imdecode (vec, 1); // Decode bytes into Mat class image.
            vec.clear();

            m.lock();
            picture_flag = true; // 사진수신을 완료하였음을 알림
            m.unlock();
            printf ("<%d's camera sent a picture completely!>\n", camId);
        }
        else {// 사진수신할 필요가 없으므로 대기
            dummy++;
        }
    }
    send_terminate_flag (clntSock, MODE_FLAG);
}

void
camera_handler (std::vector<cv::Mat>& imgs, const int& totalCam, int& WORK_FLAG, int& MODE_FLAG, std::mutex& m) {
    // Use LINGER.
    struct linger ling = {0, };
    ling.l_onoff = 1;	// linger use
    ling.l_linger = 0;	// linger timeout set

    // Creating a server socket that handles external connection requests.
    int servSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ASSERT (servSock != -1);
    
    // Create local address structure.
    struct sockaddr_in servAddr;
    memset (&servAddr, 0, sizeof(servAddr));	    // init value is 0
    servAddr.sin_family = AF_INET;		            // IPv4 addr family
    servAddr.sin_addr.s_addr = htonl (INADDR_ANY);  // accept any IP
    servAddr.sin_port = htons (PORT);	            // loacl PORT
    
    // Set LINGER: server socket
    setsockopt (servSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));

    // Bind local address.
    ASSERT (bind (servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) >= 0);
    
    // Set server socket to handle incoming requests.
    ASSERT (listen (servSock, MAXPENDING) >= 0);

    // Client socket.
    struct sockaddr_in clntAddr; // Create client address structure.
    socklen_t clntAddrLen = sizeof(clntAddr);

    int* clntSock = new int[totalCam];
    int connectedNum = 0;
    
    while (connectedNum < totalCam) { // 초기 카메라 연결
        //만약 카메라가 연결되지 않을 경우 여기에서 무한대기됨
        // Waiting for external connection.
        // Set LINGER: client socket
        setsockopt (clntSock[connectedNum], SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));
        clntSock[connectedNum] = accept (servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
        // Print Client's info.
        char clntName[INET_ADDRSTRLEN];
        if (inet_ntop (AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
            printf ("Client connected: %d\n", connectedNum+1);
        else
            puts ("Unable to get client address");
        connectedNum++;
    }

    // 이 시점에서 클라이언트소켓은 모두 clntSock 에 저장되어있는상태

    // 각 소켓별로 송수신을 담당하는 스레드 할당
    std::thread* thrs = new std::thread[totalCam];
    bool* picture_flag = new bool[totalCam]; // 여기 스레드에서 각 스레드별 사진수신여부를 총합하는 플래그
    for (int i=0; i<totalCam; i++) {
        picture_flag[i] = false; // i번째 스레드의 사진이 수신되었으면 true로 변경됨
        printf ("[thread %d] created!\n", i);
        thrs[i] = std::thread(handle_thread, std::ref(clntSock[i]), std::ref(imgs), std::ref(picture_flag[i]), std::ref(MODE_FLAG), std::ref(m));
    }

    // 각 스레드를 총괄하는 루프
    int dummy = 0;
    while (true) { // 초기 스레드들을 배분하면 이후에는 프로그램이 종료될때까지 여기에서 머뭄
        if (MODE_FLAG == TERMINATE_MODE)
            break;
        if (WORK_FLAG == GO_TAKE_PICTURE) { // 사진을 가져오라는 명령이 떨어짐

            for (int i=0; i<totalCam; i++)
                picture_flag[i] = false; // 각 스레드들에게 사진 수신하라고 알리기
            
            while (true) { // 각 스레드 사진수신 완료되었는지 조사
                bool go_to_next_work = true;
                for (int i=0; i<totalCam; i++)
                    if (!picture_flag[i]) // 아직 사진이 수신되지 않은 스레드가 있다면
                        go_to_next_work = false;
                if (go_to_next_work) // 모든 사진이 다 수신되었다면
                    break;
            }
            // 모든 스레드들이 사진수신을 완료하였으므로
            m.lock();
            WORK_FLAG = DONE_TAKE_PICTURE; // 이를 확인한 메인스레드는 Mat* imgs를 입력으로 딥러닝 시행
            m.unlock();
        }
        else { // 현재 딥러닝중이므로 대기
            dummy++;
        }
    }

    for (int i=0; i<totalCam; i++) {
        thrs[i].join(); // 각 스레드에서 handle_thread() 가 리턴될때까지 대기 (사진을 찍을 때까지 대기)
        close (clntSock[i]); // 스레드가 종료되었으면 해당 소켓을 종료
    }
    close (servSock);
    delete[] thrs;
    delete[] clntSock;
}
