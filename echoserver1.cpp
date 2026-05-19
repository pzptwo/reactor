#include "EchoServer.h"
<<<<<<< HEAD
#include <csignal>
using namespace std;
#include <signal.h> //用信号结束，由于是异步，所以作用域

EchoServer *echoserver;
=======
using namespace std;


>>>>>>> c37a5b87fc307614ee2121c871f5e51bfe1d3987
//如果不是accept4的话
//设置非阻塞的IO,????
void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL,fcntl(fd,F_GETFL)|O_NONBLOCK);
}

<<<<<<< HEAD
void Stop(int sig)
{
    printf("sig is %d\n",sig);
    //要停止服务，所以要在最上层类创建一个EchoServer::stop
    printf("echoserver stop\n");
    delete echoserver;
    printf("delete echoserver\n");
    exit(0);
}

=======
>>>>>>> c37a5b87fc307614ee2121c871f5e51bfe1d3987
int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("usage :ip port \n ");//argv顺序存
<<<<<<< HEAD
        printf("./echoserver 192.168.152.128 5000\n\n");
        return -1;
    }
    //注册信号处理
    signal(SIGTERM,Stop);   //信号15系统kill或killall命令默认发送的信号
    signal(SIGINT, Stop);   //信号2，按Ctrl+c发送的信号
    echoserver=new EchoServer(argv[1],atoi(argv[2]),3,2);  //这里的后两个参数一个是IO线程的数量，一个是WORK的
    echoserver->start();
    printf("已更改，名字\n");
=======
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
    EchoServer echoserver(argv[1],atoi(argv[2]));
    echoserver.start();

>>>>>>> c37a5b87fc307614ee2121c871f5e51bfe1d3987
    return 0;
}


<<<<<<< HEAD
//c++ struct 可以省略
=======
//c++ struct 可以省略
>>>>>>> c37a5b87fc307614ee2121c871f5e51bfe1d3987
