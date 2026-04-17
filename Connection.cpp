#include "Connection.h"

Connection::Connection(EventLoop *loop,Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_=new Channel(loop_,clientsock->fd());
    clientchannel_->setreadback(std::bind(&Channel::onMessage,clientchannel_));
    clientchannel_->useet();
    clientchannel_->enablereading();
    //clientchannel.updatechannel(clientchannel);
}

Connection::~Connection()
{
    delete clientchannel_;
    delete clientsock_;//这里相当于我拿走了new的全部
}