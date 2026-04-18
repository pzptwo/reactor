#include "Acceptor.h"
#include <iostream>
using namespace std;

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
    acceptchannel_->setreadback(std::bind(&Acceptor::newConnection,this));
    acceptchannel_->enablereading();
}  
Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}

#include "Connection.h"
//这里是新的连接请求,实在servsock 这个管道符里面
void Acceptor::newConnection()
{
    //这里用指针的原因是在堆区？还是栈忘了哈哈哈哈，就是防止这个{}结束释放clientsock
    //这里要分清楚在accept这还是属于listenfd(我的理解)
    InetAddress clientaddr;//如果用别的构造函数，会咋样
    Socket *clientsock=new Socket(servsock_->accept(clientaddr));//(相当于传进来clientfd)
    //打印一下日志
    cout << "accept client: fd=" << clientsock->fd()
            << ", ip=" << clientaddr.ip()
            << ", port=" << clientaddr.port()<< endl;
    //为新用户端连接准备读事件，并添加到epoll
    
    Connection *conn=new Connection(loop_,clientsock);//这里也没有释放，为了耦合低
}