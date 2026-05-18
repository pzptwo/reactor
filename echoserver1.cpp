#include "EchoServer.h"
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
        printf("./echoserver 192.168.152.128  5000\n\n");
        return -1;
    }

    /*
    Socket servsock(createnonblocking());
    servsock.setkeepalive(true);
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
    servsock.settcpnodelay(true);
    InetAddress servaddr(argv[1],atoi(argv[2]));
    servsock.bind(servaddr);
    servsock.listen();
    //Epoll ep;
    
    //ep.addfd(servsock.fd(),EPOLLIN);
    EventLoop loop;
    Channel *servchannel=new Channel(&loop,servsock.fd());  //这里New出来没有解决；
    servchannel->setreadback(std::bind(&Channel::newConnection,servchannel,&servsock));
    servchannel->enablereading();
    */
    // TcpServer tcpserver(argv[1],atoi(argv[2]));
    // tcpserver.start();
    EchoServer echoserver(argv[1],atoi(argv[2]),3,0);  //这里的后两个参数一个是IO线程的数量，一个是WORK的
    echoserver.start();

    return 0;
}


//c++ struct 可以省略