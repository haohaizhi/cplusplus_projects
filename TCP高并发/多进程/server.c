#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>

#include "wrap.h"

#define MAXLINE 8192
#define SERV_PORT 8000

void do_sigchild(int num)
{
    while (waitpid(0, NULL, WNOHANG) > 0);
}

int main(void)
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int i, n;
    pid_t pid;

    //临时屏蔽sigchld信号
    sigset_t myset;
    sigemptyset(&myset);
    sigaddset(&myset, SIGCHLD);
    // 自定义信号集 -》 内核阻塞信号集
    sigprocmask(SIG_BLOCK, &myset, NULL);


    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    // 设置端口复用
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, 20);

    printf("Accepting connections ...\n");
    while (1) 
    {
        cliaddr_len = sizeof(cliaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

        // 有新的连接则创建一个进程
        pid = fork();
        if (pid == 0) 
        {
            Close(listenfd);
            while (1) 
            {
                n = Read(connfd, buf, MAXLINE);
                if (n == 0) 
                {
                    printf("the other side has been closed.\n");
                    break;
                }
                printf("received from %s at PORT %d\n",
                        inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                        ntohs(cliaddr.sin_port));

                for (i = 0; i < n; i++)
                    buf[i] = toupper(buf[i]);

                Write(STDOUT_FILENO, buf, n);
                Write(connfd, buf, n);
            }
            Close(connfd);
            return 0;
        } 
        else if (pid > 0) 
        {
            struct sigaction act;
            act.sa_flags = 0;
            act.sa_handler = do_sigchild;
            sigemptyset(&act.sa_mask);
            sigaction(SIGCHLD, &act, NULL);
            // 解除对sigchld信号的屏蔽
            sigprocmask(SIG_UNBLOCK, &myset, NULL);

            Close(connfd);
        }  
        else
        {
            perr_exit("fork");
        }
    }
    return 0;
}

