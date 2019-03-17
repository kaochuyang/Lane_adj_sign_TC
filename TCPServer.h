#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <stdio.h>          // for printf() and fprintf()
#include <sys/socket.h>     // for socket(), bind(), and connect()
#include <arpa/inet.h>      // for sockaddr_in and inet_ntoa()
#include <stdlib.h>         // for atoi() and exit()
#include <string.h>         // for memset()
#include <unistd.h>         // for close()
#include <sys/time.h>       // for struct timeval {}
#include <fcntl.h>          // for fcntl()
#include <pthread.h>
#include <sys/epoll.h>

#include "PTRAFFIC92TC.h"
#include "PTRAFFIC92COMM.h"

#include "PARSETRAFFIC.h"

#include "READJOB.h"
#include "WRITEJOB.h"

#define MAXPENDING 5        // Maximum outstanding connection requests
#define MAXCLIENT  100      // Maximum client connections
#define RCVBUFSIZE 1024     // Size of receive buffer
class TCPServer
{
    public:
        TCPServer();
        virtual ~TCPServer();
        int     CreateTCPServerSocket( unsigned short );
        int     AcceptTCPConnection( int );
        int     HandleTCPClient( int );
        static void *pthread_TCPServerThread(void *);
        void tcp_thread_generate();
PARSETRAFFIC parseAABB;
    protected:
    private:
};
extern TCPServer _tcpserver;
#endif // TCPSERVER_H
