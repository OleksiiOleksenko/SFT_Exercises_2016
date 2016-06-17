#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define RCVBUFSIZE 32

#if (defined(__APPLE__) && defined(__MACH__)) || defined(MAC_OS_X)
#include <netinet/in.h>
#endif // MAC_OS_X

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
    fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n",
            prog_name);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    char *echoServPort;              /* Echo server port */
    struct addrinfo hints;           /* Hints to use IP-version agnostic TCP */
    struct addrinfo *servinfo;       /* Server info */

    char *echoString;                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    unsigned int echoStringLen;      /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() and
                                        total bytes read */

    /* Test for correct number of arguments */
    if ((argc < 3) || (argc > 4))
        usage(argv[0]);
    echoString = argv[2];

    /* if there is a port passed from command line use it! */
    if (argc == 4){
        echoServPort = argv[3];
        printf("using port %s.\n", echoServPort);
    }else
        /* if not: 7 is standard port for echo */
        echoServPort = "7";

    /* fill hints: want to use IP-version agnostic TCP protocol */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        /* don't care if IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;    /* TCP */

    /* load info about server-to-connect into servinfo */
    /* NB: servinfo is a list, but for simplicity we use only first result */
    if ((getaddrinfo(argv[1], echoServPort, &hints, &servinfo)) != 0)
        printErrorAndQuit("getaddrinfo() failed");

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(servinfo->ai_family, servinfo->ai_socktype,
            servinfo->ai_protocol)) < 0)
        printErrorAndQuit("socket() failed");

    /* Establish the connection to the echo server */
    printf("connecting to server ...\n");
    if (connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
        printErrorAndQuit("connect() failed");

    freeaddrinfo(servinfo); /* done with this structure */

    /* Send the string to the server */
    printf("sending data ...\n");
    echoStringLen = strlen(echoString);
    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
        printErrorAndQuit("send() different number of bytes than expected");

    /* Receive the same string back from the server */
    totalBytesRcvd = 0;
    printf("Received: ");                /* Setup to print the echoed string */
    while (totalBytesRcvd < echoStringLen) {
        /* Receive up to the buffer size (minus 1 to leave space for
           a null terminator) bytes from the sender */
        if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0)
            printErrorAndQuit("recv() connection closed prematurely");
        totalBytesRcvd += bytesRcvd;   /* Keep tally of total bytes */
        echoBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
        printf("%s", echoBuffer);      /* Print the echo buffer */
    }

    printf("\n");

    close(sock);
    exit(0);
}
