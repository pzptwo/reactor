#include "TcpServer.h"
#include "Socket.h"

TcpServer::TcpServer(const std::string ip,const uint16_t port)
{
    Socket *servsock=new Socket(createnonblocking());
    servsock->setkeepalive(true);
    servsock->setreuseaddr(true);
    servsock->setreuseport(true);
    servsock->settcpnodelay(true);
    InetAddress servaddr(ip,port);
    servsock->bind(servaddr);
    servsock->listen();
    //Epoll ep;
    //ep.addfd(servsock.fd(),EPOLLIN);
    //EventLoop loop;
    Channel *servchannel=new Channel(&loop_,servsock->fd());  //这里New出来没有解决；
    servchannel->setreadback(std::bind(&Channel::newConnection,servchannel,servsock));
    servchannel->enablereading();
}
TcpServer::~TcpServer()
{
    //delete loop_;
}

void TcpServer::start()
{
    loop_.run();
}