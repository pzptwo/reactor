#include <asm-generic/socket.h>
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
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
using namespace std;


//如果不是accept4的话
//设置非阻塞的IO,????
void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK);
}

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("usage :ip port \n ");//argv顺序存
        printf("./tcpepoll 192.168.229.128  5000\n\n");
        return -1;
    }

    Socket servsock(createnonblocking());
    servsock.setkeepalive(true);
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
    servsock.settcpnodelay(true);
    InetAddress servaddr(argv[1],atoi(argv[2]));
    servsock.bind(servaddr);
    servsock.listen();
    /*
    int epollfd=epoll_create(1);
    struct epoll_event ev;//这里是epoll的数据结构
    ev.data.fd=servsock.fd();
    ev.events=EPOLLIN;
    */
    Epoll ep;
    std::vector<Channel *> channels;   //这里New出来没有解决；
    //ep.addfd(servsock.fd(),EPOLLIN);
    Channel *servchannel=new Channel(&ep,servsock.fd(),true);
    //ep.updatechannel(servchannel);????
    servchannel->enablereading();
    //epoll_ctl(epollfd,EPOLL_CTL_ADD,servsock.fd(),&ev);

    //这里接下来的数据结构式epoll_wait对应的，epoll_events！！！！！！！！！！！
    // struct epoll_event evs[10]; //???
    //！！！核心，while()
    while(true)
    {
        //看看这里，可以继续封装，这里都是channel
        channels=ep.loop();  //这个函数的类型为std::vector<epoll_event>锁一前面要定义一个evs来接受

        
        for(auto &ch:channels)   //熟悉这种遍历。
        {
            ch->handleevent(&servsock);
           
        }
 
    }

    return 0;
}


//c++ struct 可以省略