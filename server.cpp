/*************************************************************************
  > File Name: server.cpp
  > Author: lyb
  > Mail: 
  > Created Time: Mon 18 Mar 2019 11:00:48 PM CST
 ************************************************************************/

#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>

#include "./include/process.h"
using namespace std;

void *pthread_child(void *arg);

/******服务器主线程******/
int main()
{

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == listenfd){
        perror("创建套接字失败");
        return -1;
    }

    struct sockaddr_in serveraddr = {0};
    struct sockaddr_in clientaddr = {0};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8080);
    serveraddr.sin_addr.s_addr = inet_addr("0.0.0.0");/*客户机可以连接本机任意ip*/

    socklen_t addrlen = sizeof(serveraddr);
    /*选择流式套接字(TCP协议)需做好握手准备*/
    int bi = bind(listenfd, (struct sockaddr *)&serveraddr, addrlen);
    if(-1 == bi){
        perror("绑定套接字失败");
        return -1;
    }
    /*开启监听队列*/
    int lis = listen(listenfd, 9);
    if(-1 == lis){
        perror("监听失败");
        return -1;
    }

    while(1){
        /*监听客户端请求，接收客户端请求*/
        cout << "监听中................." << endl;
        int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen);
        if(-1 == clientfd){
            perror("连接失败");
            return  -1;
        }
        cout << "进来的IP:" << inet_ntoa(clientaddr.sin_addr) << endl;

        /*创建子线程处理车牌识别，回收线程资源*/
        pthread_t pid;
        pthread_create(&pid, NULL, pthread_child, &clientfd);
        pthread_detach(pid);
    }
    return 0;
}

/******服务器子线程******/
void *pthread_child(void *arg)
{
    int clientfd = *(int *)arg;

    server_process(clientfd);/*线程服务函数*/
}
