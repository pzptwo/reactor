#include "Connection.h"
#include "Channel.h"

Connection::Connection(EventLoop *loop,Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_=new Channel(loop_,clientsock->fd());
    clientchannel_->setreadback(std::bind(&Channel::onMessage,clientchannel_));
    clientchannel_->setcloseback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorback(std::bind(&Connection::errorcallback,this));
    clientchannel_->useet();
    clientchannel_->enablereading();
    //clientchannel.updatechannel(clientchannel);
}

Connection::~Connection()
{
    delete clientchannel_;
    delete clientsock_;//这里相当于我拿走了new的全部
}

int  Connection::fd() const
{
    return clientsock_->fd();
}
//返回ip_
std::string Connection::ip()
{
    return clientsock_->ip();
}
//返回port_
uint16_t Connection::port()
{
    return clientsock_->port();
}   

void Connection::closecallback()
{
    closecallback_(this);
}

void Connection::errorcallback()
{
    errorcallback_(this);
}

void Connection::setcloseback(std::function<void (Connection *)> fn)
{
    closecallback_=fn;
}

void Connection::seterrorback(std::function<void (Connection *)>fn)
{
    errorcallback_=fn;
}