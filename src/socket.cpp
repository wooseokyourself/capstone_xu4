#include "socket.hpp"

ssize_t
Recv (int sock, const void *buf, ssize_t size, ssize_t unit) {
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

vector<unsigned char>
RecvBuffer () {
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
    int clntSock;
    struct sockaddr_in clntAddr; // Create client address structure.
    socklen_t clntAddrLen = sizeof(clntAddr);
    // Set LINGER: client socket
    setsockopt (clntSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));

    // Waiting for external connection.
	#ifdef DEBUG
	printf ("wait client...\n");
	#endif
    clntSock = accept (servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);


    // At this time @clntSock is connected with real external client.


    // Print Client's info.
    char clntName[INET_ADDRSTRLEN];
    if (inet_ntop (AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
	    printf ("Client connected %s%d\n", clntName, ntohs(clntAddr.sin_port));
    else
	    puts ("Unable to get client address");


	int recvd;

	// Receive @vec.size()
	ssize_t bufSize;
	recvd = Recv (clntSock, &bufSize, sizeof(bufSize), sizeof(size_t *));

	// Receive @vec
	vector<unsigned char> vec(bufSize);
	recvd = Recv (clntSock, &vec[0], bufSize, sizeof(unsigned char));

	ASSERT (recvd == vec.size() * sizeof(unsigned char));

	close (servSock);
    close (clntSock);

    return vec;
}
