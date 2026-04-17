#include "Acceptor.h"

//
Acceptor::Acceptor(EventLoop *loop,const std::string ip,const uint16_t port):loop_(loop)
{
    servsock_=new Socket(createnonblocking());
    servsock_->setkeepalive(true);
    servsock_->setreuseaddr(true);
    servsock_->setreuseport(true);
    servsock_->settcpnodelay(true);
    InetAddress servaddr(ip,port);
    servsock_->bind(servaddr);
    servsock_->listen();
    //Epoll ep;
    //ep.addfd(servsock.fd(),EPOLLIN);
    //EventLoop loop;
    acceptchannel_=new Channel(loop_,servsock_->fd());
    acceptchannel_->setreadback(std::bind(&Channel::newConnection,acceptchannel_,servsock_));
    acceptchannel_->enablereading();
}  
Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}