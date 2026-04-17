#pragma once 

#include <cerrno>
#include <endian.h>
#include <fcntl.h>
#include <iterator>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>          
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>      // TCP_NODELAY需要包含这个头文件。
#include <iostream>
#include "InetAddress.h"
#include "Acceptor.h"
//TCP 网络服务类
class TcpServer
{
    private:
       //一个Tcpserver对应多个eventloop（多线程） ，所以就是TcpServer管理loop
        EventLoop loop_;    
        Acceptor *acceptor; //一个Tcpserver对应一个Accept对象
    public:
        //由于要完成bind所以需要传参
        TcpServer(const std::string ip,const uint16_t port);
        ~TcpServer();

        void start();   //调用loop的run,相当于*loop_的接口，裕兴事件循环
};