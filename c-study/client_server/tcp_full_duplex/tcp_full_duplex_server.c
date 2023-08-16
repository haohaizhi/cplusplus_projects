/**
 * @file
 * @brief Server-side implementation of [TCP Full Duplex Communication]
 * @see tcp_full_duplex_server.c
 *
 * @details
 * 该算法基于简单的TCP客户端和服务器模型。当然，并不是服务器只发送和客户端只接收数据，服务器和客户端可以同时发送和接收数据。
 * 这是通过使用' fork '函数调用来实现，以便在服务器端调用子进程，子进程可以接收数据，父进程可以发送数据，并在客户端子进程可以发送数据，
 * 父进程可以接收数据，它运行一个无限循环，可以无限期地发送和接收消息，直到用户退出循环。这样，就形成了全双工的通信形式。
 * 可以用TCP服务器-客户端模型和套接字编程来表示。
 */

#include <arpa/inet.h>  /// For the type in_addr_t and in_port_t
#include <netdb.h>  /// For structures returned by the network database library - formatted internet addresses and port numbers
#include <netinet/in.h>  /// For in_addr and sockaddr_in structures
#include <stdint.h>      /// For specific bit size values of variables
#include <stdio.h>  /// Variable types, several macros, and various functions for performing input and output
#include <stdlib.h>  /// Variable types, several macros, and various functions for performing general functions
#include <string.h>  /// Various functions for manipulating arrays of characters
#include <sys/socket.h>  /// For macro definitions related to the creation of sockets
#include <sys/types.h>  /// For definitions to allow for the porting of BSD programs
#include <unistd.h>  /// For miscellaneous symbolic constants and types, and miscellaneous functions

#define PORT 10000  /// Define port over which communication will take place

/**
 * @brief Utility function used to print an error message to `stderr`.
 * It prints `str` and an implementation-defined error
 * message corresponding to the global variable `errno`.
 * @returns void
 */
void error()
{
    perror("Socket Creation Failed");
    exit(EXIT_FAILURE);
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main()
{
    /** Variable Declarations */
    uint32_t sockfd,
        conn;  ///< socket descriptors - Like file handles but for sockets
    char recvbuff[1024],
        sendbuff[1024];  ///< character arrays to read and store string data
                         /// for communication

    struct sockaddr_in server_addr,
        client_addr;  ///< basic structures for all syscalls and functions that
                      /// deal with internet addresses. Structures for handling
                      /// internet addresses
    socklen_t ClientLen;  /// size of address

    /**
     * The TCP socket is created using the socket function
     *
     * AF_INET (Family) - it is an address family that is used to designate the
     * type of addresses that your socket can communicate with
     *
     * SOCK_STREAM (Type) - Indicates TCP Connection - A stream socket provides
     * for the bidirectional, reliable, sequenced, and unduplicated flow of data
     * without record boundaries. Aside from the bidirectionality of data flow,
     * a pair of connected stream sockets provides an interface nearly identical
     * to pipes
     *
     * 0 (Protocol) - Specifies a particular protocol to be used with the
     * socket. Specifying a protocol of 0 causes socket() to use an unspecified
     * default protocol appropriate for the requested socket type
     */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error();  ///< Error if the socket descriptor has a value lower than 0 -
                  /// socket wasnt created
    }

    /**
     * Server Address Information
     *
     * The bzero() function erases the data in the n bytes of the memory
     * starting at the location pointed to, by writing zeros (bytes
     * containing '\0') to that area
     *
     * We bind the server_addr to the internet address and port number thus
     * giving our socket an identity with an address and port where it can
     * listen for connections
     *
     * htons - The htons() function translates a short integer from host byte
     * order to network byte order
     *
     * htonl - The htonl() function translates a long integer from host byte
     * order to network byte order
     *
     * These functions are necessary so that the binding of address and port
     * takes place with data in the correct format
     */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    printf("Server is running...\n");

    /**
     * This binds the socket descriptor to the server thus enabling the server
     * to listen for connections and communicate with other clients
     */
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error();  /// If binding is unsuccessful
    }
    /**
     * This is to listen for clients or connections made to the server
     *
     * The limit is currently at 5 but can be increased to listen for
     * more connections
     *
     * It listens to connections through the socket descriptor
     */
    listen(sockfd, 5);

    printf("Server is listening...\n");

    /**
     * When a connection is found, a socket is created and connection is
     * accepted and established through the socket descriptor
     */
    conn = accept(sockfd, (struct sockaddr *)NULL, NULL);

    printf("Server is connected...\n");

    /**
     * Communication between client and server
     *
     * The bzero() function erases the data in the n bytes of the memory
     * starting at the location pointed to, by writing zeros (bytes
     * containing '\0') to that area. The variables are emptied and then
     * ready for use
     *
     * The fork function call is used to create a child and parent process
     * which run and execute code simultaneously
     *
     * The child process is used to receive data and after doing so
     * sleeps for 5 seconds to wait for the parent to send data
     *
     * The parent process is used to send data and after doing so
     * sleeps for 5 seconds to wait for the child to receive data
     *
     * The server and client can communicate indefinitely till one of them
     * exits the connection
     *
     * Since the exchange of information between the server and client takes
     * place simultaneously this represents FULL DUPLEX COMMUNICATION
     */
    pid_t pid;
    pid = fork();
    if (pid == 0)  /// Value of 0 is for child process
    {
        while (1)
        {
            bzero(&recvbuff, sizeof(recvbuff));
            recv(conn, recvbuff, sizeof(recvbuff), 0);
            printf("\nCLIENT : %s\n", recvbuff);
            sleep(5);
            // break;
        }
    }
    else  /// Parent process
    {
        while (1)
        {
            bzero(&sendbuff, sizeof(sendbuff));
            printf("\nType message here: ");
            fgets(sendbuff, 1024, stdin);
            send(conn, sendbuff, strlen(sendbuff) + 1, 0);
            printf("\nMessage Sent!\n");
            sleep(5);
            // break;
        }
    }

    /// Close socket
    close(sockfd);
    printf("Server is offline...\n");
    return 0;
}
