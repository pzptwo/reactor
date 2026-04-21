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
#include "EventLoop.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "Socket.h"
#include "Connection.h"
#include <map>
//TCP 网络服务类
class TcpServer
{
    private:
       //一个Tcpserver对应多个eventloop（多线程） ，所以就是TcpServer管理loop
        EventLoop loop_;    
        Acceptor *acceptor_; //一个Tcpserver对应一个Accept对象
        std::map<int,Connection*> conns_;   //一个Tcpserver对应多个Connection对象，存放在map容器里面

        std::function<void (Connection *)> newConnectioncb_;
        std::function<void (Connection *)> closecb_;
        std::function<void (Connection *)> errorcb_;
        std::function<void (Connection *,std::string )> slovemessagecb_;
        std::function<void (Connection *)> sendCompletecb_;
        std::function<void (EventLoop *)> epolltimeoutcb_;
    public:
        //由于要完成bind所以需要传参
        TcpServer(const std::string ip,const uint16_t port);
        ~TcpServer();

        void start();   //调用loop的run,相当于*loop_的接口，裕兴事件循环
        void newConnection(Socket *clientsock);   //处理新连接上来的
        void closecallback(Connection *conn);   //我感觉取名有点问题
        void errorcallback(Connection * conn);
        void slovemessage(Connection* conn,std::string message);

        //对于最上层，要知道，数据已经发送完毕
        void sendComplete(Connection *conn);

        //对于最上层，要知道，是否超时,当Channel为空时，而且因为在eventloop里面，所以要说明是哪一个loop
        void epolltimeout(EventLoop *loop);

        void setnewConnectioncb(std::function<void (Connection *)>fn);
        void setclosecb(std::function<void (Connection *)>fn);
        void seterrorcb(std::function<void (Connection *)>fn);
        void setslovemessagecb(std::function<void (Connection *,std::string)>fn);
        void setsendCompletecb(std::function<void (Connection *)>fn);
        void setepolltimeoutcb(std::function<void (EventLoop *)>fn);
};