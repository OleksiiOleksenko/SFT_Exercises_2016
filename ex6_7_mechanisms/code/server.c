#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>

#include <sys/wait.h>
#include <sys/resource.h>

#if (defined(__APPLE__) && defined(__MACH__)) || defined(MAC_OS_X)
#include <netinet/in.h>
#endif // MAC_OS_X

#define MAXPENDING 5
#define RCVBUFSIZE 1024*1024
#define MEMLIMIT 32*1024*1024

int fd[2];  // pipe fd

int backup() {
    int restarts = 0, ret = 0;

    for (;; restarts++) {

        if (pipe(fd) < 0) {
            fd[0] = -1;
            fd[1] = -1;
            printf("Error creating pipe\n");
            ret = -1;
        } else {
            /* integer overflow -> trigger graceful degradation */
            if (restarts < 0)
                ret = -1;
            else ret = fork();
        }

        if (ret == 0) {// child returns
            close(fd[1]);
            printf("Restart %d\n", restarts);
            printf("New echo server with PID: %d\n", (int) getpid());
            return restarts;
        }

        if ( ret < 0 ) {
            printf("Graceful degradation .. running without backup\n");
            return ret;
        }

        close(fd[0]);
        printf("Waiting %d for %d\n", (int) getpid(), ret);
        while (ret != waitpid(ret,0,0));
    }
    return 0;
}

/*
 * This function returns true (as int: 1) if the pipe between both
 * processes has been closed.
 */
int isPipeClosed() {
    struct pollfd pfds;
    int time = 0;

    pfds.fd = fd[0];
    pfds.events = POLLHUP | POLLERR | POLLIN; // POLLIN might be enough..
    pfds.revents = 0;

    if(poll(&pfds, 1, time) < 0) {
        printf("Poll failed\n");
    }
    if (pfds.revents) {
        printf("Pipe failed (%d)\n", pfds.revents);
        close (fd[0]);
        return 1;
    }
    return 0;
}


/*
 * This is just a little helper
 * function to write an error
 * message and quit the program.
 */
void printErrorAndQuit(char *msg){
    perror(msg);
    exit(1);
}

/*
 * For each client connection this function echoes the message.
 * The client socket is closed if the message is sent.
 */
void echoMessage(int clntSocket){
//    char echoBuffer[RCVBUFSIZE];
    char *echoBuffer = malloc(RCVBUFSIZE);
    int recvMsgSize;

    /*receive message*/
    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        printErrorAndQuit("could not receive message!");
    /*send message back to client*/
    while (recvMsgSize > 0){
        if (send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
            printErrorAndQuit("send() failed");
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
            printErrorAndQuit("recv() failed");
    }
    close(clntSocket);
}

/*
 * Get relevant socket address struct depending on IP version
 */
void *getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * This is just a little helper
 * function to show how the program
 * is started from command line.
 */
void usage(char *prog_name){
    fprintf(stderr, "Usage:  %s <Server Port>\n", prog_name);
    exit(1);
}

int main(int argc, char *argv[])
{
    struct rlimit rlim;

    /* set resource limits */
    rlim.rlim_cur = MEMLIMIT;
    rlim.rlim_max = MEMLIMIT;
    if (0 != setrlimit (RLIMIT_AS, &rlim))
        printErrorAndQuit ("setrlimit failed");
    int servSock;                    /* Socket descriptor for server */
    struct addrinfo hints;           /* Hints to use IP-version agnostic TCP */
    struct addrinfo *servinfo;       /* Server info */

    /* Test for correct number of arguments */
    if (argc != 2)
        usage(argv[0]);

    /* fill hints: want to use IP-version agnostic TCP protocol */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        /* don't care if IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;    /* TCP */
    hints.ai_flags = AI_PASSIVE;        /* use local address */

    /* load info about server into servinfo */
    /* NB: servinfo is a list, but for simplicity we use only first result */
    if ((getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0)
        printErrorAndQuit("getaddrinfo() failed");

    /* Create socket for incoming connections */
    if ((servSock = socket(servinfo->ai_family, servinfo->ai_socktype,
            servinfo->ai_protocol)) < 0)
        printErrorAndQuit("socket() failed");

    /* Bind to the local address */
    if (bind(servSock, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
        printErrorAndQuit("bind() failed");

    freeaddrinfo(servinfo); /* done with this structure */

    /* listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        printErrorAndQuit("listen() failed");


    char s[INET6_ADDRSTRLEN];      /* Human-readable IP address */
    int clntSock;                  /* Socket descriptor for client */
    unsigned int clntLen;          /* Length of client address data structure */
    struct sockaddr_storage echoClntAddr;   /* Client address */

    /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);

#if 1
    int ft = backup();
#endif

    for (;;) { 
        /* Wait for a client to connect */
        struct timeval tv = { .tv_sec = 5, .tv_usec = 0 };

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(servSock, &rfds);

        /* wait some seconds for connections
           then process request or ...
           have a look if parent is still alive*/
        int retval = select(servSock+1, &rfds, NULL, NULL, &tv);

        if (retval) {
            if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
                                   &clntLen)) < 0)
                printErrorAndQuit("accept() returned error");

            /* clntSock is connected to a client! */

            inet_ntop(echoClntAddr.ss_family,
                getInAddr((struct sockaddr *)&echoClntAddr),
                s, sizeof s);
            printf("Handling client %s\n", s);

            echoMessage(clntSock);
        }

        if (ft < 0 || isPipeClosed()) { ft = backup(); }
    }

    /* this point is never reached */
}
