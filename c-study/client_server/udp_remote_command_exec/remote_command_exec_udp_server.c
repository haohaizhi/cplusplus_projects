/**
 * @file
 * @brief Server-side implementation of [Remote Command Execution Using UDP]
 * @see remote_command_exec_udp_server.c
 *
 * @details
 * 该算法基于简单的UDP客户端和服务器模型。它运行一个无限循环，获取用户输入并将其发送到服务器
 * 来执行。服务器接收命令并执行它们直到用户退出循环
 * 通过这种方式，远程命令执行，使用UDP显示
 * 使用服务器-客户端模型和套接字编程
 */

#include <arpa/inet.h>  /// For the type in_addr_t and in_port_t
#include <errno.h>      /// To indicate what went wrong if an error occurs
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
    uint32_t
        sockfd;  ///< socket descriptors - Like file handles but for sockets
    char recv_msg[1024],
        success_message[] =
            "Command Executed Successfully!\n";  ///< character arrays to read
                                                 /// and store string data
                                                 /// for communication & Success
                                                 /// message

    struct sockaddr_in server_addr,
        client_addr;  ///< basic structures for all syscalls and functions that
                      /// deal with internet addresses. Structures for handling
                      /// internet addresses
    socklen_t clientLength = sizeof(client_addr);  /// size of address

    /**
     * The UDP socket is created using the socket function.
     *
     * AF_INET (Family) - it is an address family that is used to designate the
     * type of addresses that your socket can communicate with
     *
     * SOCK_DGRAM (Type) - Indicates UDP Connection - UDP does not require the
     * source and destination to establish a three-way handshake before
     * transmission takes place. Additionally, there is no need for an
     * end-to-end connection
     *
     * 0 (Protocol) - Specifies a particular protocol to be used with the
     * socket. Specifying a protocol of 0 causes socket() to use an unspecified
     * default protocol appropriate for the requested socket type.
     */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        error();
    }

    /**
     * Server Address Information
     *
     * The bzero() function erases the data in the n bytes of the memory
     * starting at the location pointed to, by writing zeros (bytes
     * containing '\0') to that area.
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

    /**
     * This binds the socket descriptor to the server thus enabling the server
     * to listen for connections and communicate with other clients
     */
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error();  /// If binding is unsuccessful
    }

    printf("Server is Connected Successfully...\n");

    /**
     * Communication between client and server
     *
     * The bzero() function erases the data in the n bytes of the memory
     * starting at the location pointed to, by writing zeros (bytes
     * containing '\0') to that area. The variables are emptied and then
     * ready for use
     *
     * The server receives data from the client which is a command. It then
     * executes the command.
     *
     * The client then receives a response from the server when the
     * command has been executed
     *
     * The server and client can communicate indefinitely till one of them
     * exits the connection
     *
     * The client sends the server a command which it executes thus showing
     * remote command execution using UDP
     */
    while (1)
    {
        bzero(recv_msg, sizeof(recv_msg));
        recvfrom(sockfd, recv_msg, sizeof(recv_msg), 0,
                 (struct sockaddr *)&client_addr, &clientLength);
        printf("Command Output: \n");
        system(recv_msg);
        printf("Command Executed\n");
        sendto(sockfd, success_message, sizeof(success_message), 0,
               (struct sockaddr *)&client_addr, clientLength);
    }

    /// Close socket
    close(sockfd);
    printf("Server is offline...\n");
    return 0;
}
