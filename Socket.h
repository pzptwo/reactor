#pragma once 
#include<sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"
int createnonblocking();//创建一个非阻塞的socket;

class Socket
{
    private:
       const int fd_;//一个socket对应一个fd，Socket持有的fd，在构造函数中传进来
    public:
        Socket(int fd); //构造函数，传入一个准备好的fd

        ~Socket();  //在析构函数中关闭fd
        int fd() const;
        void setreuseaddr(bool on);
        void setreuseport(bool on);
        void settcpnodelay(bool on);
        void setkeepalive(bool on);
        void bind(const InetAddress &servaddr);  //服务端的socket将调用此函数
        void listen(int nn=128);
        int accept(InetAddress &clientaddr);    //服务端的socket将调用此函数

};