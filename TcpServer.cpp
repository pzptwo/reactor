#include "TcpServer.h"
#include "Acceptor.h"

TcpServer::TcpServer(const std::string ip,const uint16_t port)
{
    /*
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

    */
    acceptor=new Acceptor(&loop_,ip,port);
}
TcpServer::~TcpServer()
{
    //delete loop_;
    delete acceptor;
}

void TcpServer::start()
{
    loop_.run();
}