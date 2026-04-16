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
    Channel *servchannel=new Channel(&ep,servsock.fd());
    //ep.updatechannel(servchannel);????
    servchannel->enablereading();
    //epoll_ctl(epollfd,EPOLL_CTL_ADD,servsock.fd(),&ev);

    //这里接下来的数据结构式epoll_wait对应的，epoll_events！！！！！！！！！！！
    // struct epoll_event evs[10]; //???
    //！！！核心，while()
    while(true)
    {
        channels=ep.loop();  //这个函数的类型为std::vector<epoll_event>锁一前面要定义一个evs来接受
        for(auto &ch:channels)   //熟悉这种遍历。
        {
            //根据不同的数据结构分类
            if(ch->revent()&EPOLLRDHUP)
            {
                //这里表示对方已关闭
                cout<<"client(eventfd)"<<ch->fd()<<"disconnect"<<endl;
                close(ch->fd());
            }
            else if(ch->revent()&(EPOLLIN|EPOLLPRI))
            {
                //服务器这边要分为两种
                if(ch==servchannel)//这里就要开始accept了,这里就会有新的fd了
                {
                   
                    //这里用指针的原因是在堆区？还是栈忘了哈哈哈哈，就是防止这个{}结束释放clientsock
                    //这里要分清楚在accept这还是属于listenfd(我的理解)
                    InetAddress clientaddr;//如果用别的构造函数，会咋样
                    Socket *clientsock=new Socket(servsock.accept(clientaddr));//(相当于传进来clientfd)
                    //打印一下日志
                    cout << "accept client: fd=" << clientsock->fd()
                            << ", ip=" << clientaddr.ip()
                            << ", port=" << clientaddr.port()<< endl;
                    //为新用户端连接准备读事件，并添加到epoll
                    /*
                    ev.data.fd=clientsock->fd();
                    ev.events=EPOLLIN|EPOLLET;//边缘触发

                    epoll_ctl(epollfd,EPOLL_CTL_ADD,clientsock->fd(),&ev);//这里的参数意义
                    */
                    //ep.addfd(clientsock->fd(),EPOLLIN|EPOLLET);
                    
                    Channel *clientchannel=new Channel(&ep,clientsock->fd());
                    clientchannel->useet();
                    clientchannel->enablereading();
                    //clientchannel.updatechannel(clientchannel);
                }
                else 
                {
                    char buffer[1024];
                    while(true)
                    {
                        bzero(buffer,sizeof(buffer));//这个函数与memset的区别
                        ssize_t nread=read(ch->fd(),buffer,sizeof(buffer));//这个函数的赋值？？？

                        if(nread>0)
                        {
                            cout << "recv from client(eventfd=" <<ch->fd() << "): " << buffer << endl;
                            //这里服务器在接收到数据，后的处理方式
                            send(ch->fd(),buffer,strlen(buffer),0);
                        }
                        //错误有好几种，有些需要排除
                        else if(nread==-1&&errno==EINTR)     // 读取数据的时候被信号中断，继续读取。
                        {
                            continue;
                        }
                        else if(nread==-1&&((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                        {
                            break;
                        }
                        else if(nread==0)   // 客户端连接已断开，和上面的重复了
                        {
                            printf("client(eventfd=%d) disconnected.\n",ch->fd());
                            close(ch->fd());            // 关闭客户端的fd。
                            break;
                        }
                    }
                }
            }
            //后面的events类型
            else if(ch->revent()&EPOLLOUT) // 有数据需要写，暂时没有代码，以后再说。
            {

            }
            else 
            {
                 printf("client(eventfd=%d) error.\n",ch->fd());
                close(ch->fd());            // 关闭客户端的fd。
            }
        }
    }

    return 0;
}


//c++ struct 可以省略