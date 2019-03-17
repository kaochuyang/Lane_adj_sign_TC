#include "TCPServer.h"
TCPServer _tcpserver;
TCPServer::TCPServer()
{
    //ctor
}

TCPServer::~TCPServer()
{
    //dtor
}
void * TCPServer::pthread_TCPServerThread(void *)
{
    int             tcpServSock;     // Socket descriptors for TCP server
    long            timeout;         // Timeout value given on command-line

//    int cliSock[MAXCLIENT];          // Client Socket Set

    int running = 1;                 // 1 if server should be running; 0 otherwise

    unsigned short portNo;           // Actual port number

    int i, j;                        // For loop use

    int                     epfd;                   // EPOLL File Descriptor.
    struct epoll_event      ev;                     // Used for EPOLL.
    struct epoll_event      events[120];            // Used for EPOLL.
    int                     noEvents;               // EPOLL event number.


    timeout =60;//atol( argv[1] );      // First arg: Timeout
    portNo =smem.GetLocalIP1(5);// atoi( argv[2] );

    // Create epoll file descriptor.
    // MAXCLIENT + 3 = MAXCLIENT + TCP + UDP +STD_IN.
    epfd = epoll_create( MAXCLIENT + 3 );

    // Create port socket
    tcpServSock =_tcpserver.CreateTCPServerSocket( portNo );
//    udpServSock = _tcpserver.CreateUDPServerSocket( portNo );

    // Add to the epoll
    ev.data.fd = tcpServSock;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    epoll_ctl( epfd, EPOLL_CTL_ADD, tcpServSock, &ev );

    // Add STDIN into the EPOLL set.
    ev.data.fd = STDIN_FILENO;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl( epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev );

    // Initialize the client socket pool
    for( i = 0 ; i < MAXCLIENT ; i++ )
    {
//        cliSock[i] = -1;
smem.setCliSock(i,-1);
    }

    printf( "Starting server:  Hit return to shutdown\n" );
    while ( running )
    {
        // Wait for events.
        // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
        // Specifying a timeout of -1 makes epoll_wait() wait indefinitely.
        noEvents = epoll_wait( epfd, events, FD_SETSIZE , ( timeout * 1000 ) );

        if ( noEvents <= 0 )
        {
            printf("No echo requests for %ld secs...Server still alive\n", timeout);
            continue;
        }

        for( i = 0 ; i < noEvents; i++ )
        {
            if( events[i].events & EPOLLIN && STDIN_FILENO == events[i].data.fd )
            {
                printf("Shutting down server\n");
                getchar();
                running = 0;
                continue;
            }
            else if( ( events[i].events & EPOLLIN ) &&
                     ( events[i].data.fd == tcpServSock ) )
            {
                for( i = 0 ; i < MAXCLIENT ; i++ )
                {
                    if( smem.getCliSock(i) < 0 )
                    {
//                        cliSock[i]
                        smem.setCliSock(i ,_tcpserver.AcceptTCPConnection( tcpServSock ));

                        // Add the client socket to the epoll fdset.
                        ev.data.fd = smem.getCliSock(i);
//                        cliSock[i];
                        ev.events = EPOLLIN | EPOLLET;
                        epoll_ctl( epfd, EPOLL_CTL_ADD, smem.getCliSock(i), &ev );

                        i = MAXCLIENT;
                    }
                }
            }
            else if ( events[i].events & EPOLLIN )
            {
                if( _tcpserver.HandleTCPClient( events[i].data.fd ) == 0 )
                {
                    printf( "Connection %d Shudown.\n", events[i].data.fd );

                    // We do not need to del fd from the epfd set.
                    // Close the fd will remove it from epfd automatically.

                    close( events[i].data.fd );

                    for( j = 0 ; j < MAXCLIENT; j++ )
                    {
                        if( //                        cliSock[j]
                        smem.getCliSock(j)== events[i].data.fd )
                        {
//                            cliSock[j] = -1;
smem.setCliSock(j,-1);
                            j = MAXCLIENT;
                        }
                    }
                }

            }
        }
    }

    // Close sockets
    close( tcpServSock );


    for ( i = 0; i < MAXCLIENT; i++ )
    {
        if(
//        cliSock[i]
        smem.getCliSock(i)> 0 )
        {
            close( smem.getCliSock(i));
        }
    }

    // Free list of sockets
    close( epfd );

    return 0;



}
void TCPServer::tcp_thread_generate()
{
    pthread_t tid;
    printf("TCP thread Starting!\n");
    printf("\nMAIN:                 pid=%d\n",getpid());
    pthread_attr_t attr;
    pthread_attr_init( & attr );
    pthread_attr_setdetachstate( & attr, PTHREAD_CREATE_DETACHED );
    //  pthread_create( & ( CSTC::_stc_thread_detector_info ), & attr, & ( CSTC::_stc_thread_detector_info_func ), NULL );
    pthread_create( & ( tid ), & attr, & ( TCPServer::pthread_TCPServerThread), NULL );
    //pthread_record=tid;
    pthread_attr_destroy( & attr );

}
int TCPServer::CreateTCPServerSocket(unsigned short port)
{
    int sock;                        // socket to create
    struct sockaddr_in echoServAddr; // Local address

    // Create socket for incoming connections
    if ( ( sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 )
    {
        perror( "socket() failed" );
        exit(1);
    }

    // Construct local address structure
    memset( &echoServAddr, 0, sizeof( echoServAddr ) );     // Zero out structure
    echoServAddr.sin_family = AF_INET;                      // Internet address family
    echoServAddr.sin_addr.s_addr = htonl( INADDR_ANY );     // Any incoming interface
    echoServAddr.sin_port = htons( port );                  // Local port

    // Bind to the local address
    if ( bind(sock, (struct sockaddr *) &echoServAddr, sizeof( echoServAddr ) ) < 0 )
    {
        perror( "bind() failed" );
        exit(1);
    }

    // Mark the socket so it will listen for incoming connections
    if ( listen( sock, MAXPENDING ) < 0 )
    {
        perror( "listen() failed" );
        exit(1);
    }

    return sock;
}

int TCPServer::AcceptTCPConnection( int servSock )
{
    int                 clntSock;     // Socket descriptor for client
    struct sockaddr_in  echoClntAddr; // Client address
    unsigned int        clntLen;      // Length of client address data structure

    // Set the size of the in-out parameter
    clntLen = sizeof( echoClntAddr );

    // Wait for a client to connect
    if ( ( clntSock = accept( servSock, (struct sockaddr *) &echoClntAddr, &clntLen ) ) < 0 )
    {
        perror("accept() failed");
        exit(1);
    }

    // clntSock is connected to a client!

    printf( "Handling client %s:%d (%d)\n", inet_ntoa( echoClntAddr.sin_addr ),
            ntohs( echoClntAddr.sin_port ), clntSock );

    return clntSock;
}

int  TCPServer::HandleTCPClient( int clntSocket )
{
    BYTE    echoBuffer[RCVBUFSIZE];        // Buffer for echo string
    int     recvMsgSize;                   // Size of received message
int readSelectLength;
    bzero( echoBuffer, RCVBUFSIZE );

    // Receive message from client
    if ( ( recvMsgSize = recv( clntSocket, echoBuffer, RCVBUFSIZE, 0 ) ) < 0 )
    {
        perror("recv() failed");
        exit(1);
    }

    // Send received string and receive again until end of transmission
    if ( recvMsgSize > 0 )      // zero indicates end of transmission
    {
        printf( "Recv(%d): %s\n", recvMsgSize, echoBuffer );


        readSelectLength=recvMsgSize;

                /* printf("receive message  =");
                 for(int i=0;i<readSelectLength;i++)printf("%x ",echoBuffer[i]);
                 printf("\n");*/                                //OT20110526
                smem.vSetLastGetProtocolTime();

                parseAABB.ParseBlock(readSelectLength,echoBuffer,smem.centerSocket.messageIn,&smem.centerSocket.lastPacketIndex,&smem.centerSocket.maxMessageIndex);
                parseAABB.CheckSum(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                //enable vJudgeProtocolAndCheckLength
                parseAABB.vJudgeProtocolAndCheckLength(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                parseAABB.DecideProtocol(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn,smem.centerSocket.GetConnDevice());
                parseAABB.CheckReasonable(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                parseAABB.AssignLcn(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                readJob.SetInterfaceAndReadFlag(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn,cUDP);
                readJob.vCheckIfBcastingForwardToUDP(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                readJob.CheckLcn(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                readJob.SetCenterComm(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                readJob.DoWorkByMESSAGEIN(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                parseAABB.EchoToGUI(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn,"192.168.1.101:6003");
                parseAABB.MoveLastData(&smem.centerSocket.maxMessageIndex,&smem.centerSocket.lastPacketIndex,smem.centerSocket.messageIn);


                // Echo message back to client
//                if ( send( clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize )
//                {
//                    perror( "send() failed" );
//                    exit(1);
//                }
            }

            return recvMsgSize;
        }
