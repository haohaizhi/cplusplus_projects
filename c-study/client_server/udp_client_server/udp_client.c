/**
 * @file
 * @brief Client side implementation of UDP client-server model
 * @see client_server/udp_server.c
 */
#ifdef _WIN32                            // if compiling for Windows
#define _WINSOCK_DEPRECATED_NO_WARNINGS  // will make the code invalid for next
                                         // MSVC compiler versions
#include <winsock2.h>
#define close closesocket /**< map BSD name to Winsock */
#else                     // if not windows platform
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080    /**< port number to connect to */
#define MAXLINE 1024 /**< maximum characters per line */

#ifdef _WIN32
/** Cleanup function will be automatically called on program exit */
void cleanup() { WSACleanup(); }
#endif

/** Driver code */
int main()
{
#ifdef _WIN32
    // when using winsock2.h, startup required
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
    {
        perror("WSA Startup error: \n");
        return 0;
    }

    atexit(cleanup);  // register at-exit function
#endif

    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client";
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n;
    unsigned int len;

    sendto(sockfd, (const char *)hello, strlen(hello), 0,
           (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Hello message sent.\n");

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
                 (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);

    close(sockfd);
    return 0;
}
