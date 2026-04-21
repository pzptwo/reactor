#include "TcpServer.h"
#include "Acceptor.h"
#include "Connection.h"
#include "Epoll.h"
#include <functional>
#include <iostream>
using namespace std;


TcpServer::TcpServer(const std::string ip,const uint16_t port)
{
    acceptor_=new Acceptor(&loop_,ip,port);
    //刚刚启动，还没有socket所以不能传clientsock
    acceptor_->setnewConnectioncb(std::bind(&TcpServer::newConnection,this,std::placeholders::_1));
    loop_.setepolltimeoutcb(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
}
TcpServer::~TcpServer()
{
    //delete loop_;
    delete acceptor_;
    for(auto &aa:conns_)
    {
        delete aa.second;
    }
}

void TcpServer::start()
{
    loop_.run();
}


//处理新连接上来的
void TcpServer::newConnection(Socket *clientsock)
{
    Connection *conn=new Connection(&loop_,clientsock);//这里也没有释放，为了耦合低
    conn->setcloseback(std::bind(&TcpServer::closecallback,this,std::placeholders::_1));
    conn->seterrorback(std::bind(&TcpServer::errorcallback,this,std::placeholders::_1));
    conn->setslovecb(std::bind(&TcpServer::slovemessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendCompletecb(std::bind(&TcpServer::sendComplete,this,std::placeholders::_1));
    // cout << "new Connection: fd=" << clientsock->fd()
    //         << ", ip=" << clientsock->ip()
    //         << ", port=" <<  clientsock->port()<< endl;
    conns_[conn->fd()]=conn;

    if(newConnectioncb_)newConnectioncb_(conn); //新建连接的时候，要等连接建立好才开始
}

void TcpServer::closecallback(Connection * conn)
{
    if(closecb_) closecb_(conn); //关闭连接的时候，则是需要先回调
    //printf("client(eventfd=%d) disconnected.\n",conn->fd());
    //close(fd()); 
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorcallback(Connection* conn)
{
    if(errorcb_)errorcb_(conn);
    //printf("client(eventfd=%d) error.\n",conn->fd());
    //close(conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::slovemessage(Connection* conn,std::string message)
{
    if(slovemessagecb_) slovemessagecb_(conn,message);
}

void TcpServer::sendComplete(Connection *conn)
{
    
    //参数一定要conn，表明是这个连接的
    //printf("send complete;\n");
    if(sendCompletecb_) sendCompletecb_(conn);
   
}

void TcpServer::epolltimeout(EventLoop *loop)
{
    
    //printf("epoll timeout\n");
    if(epolltimeoutcb_) epolltimeoutcb_(loop);//可以在这里面增加逻辑
    
}

void TcpServer::setnewConnectioncb(std::function<void (Connection *)>fn)
{
    newConnectioncb_=fn;
}
void TcpServer::setclosecb(std::function<void (Connection *)>fn)
{
    closecb_=fn;
}
void TcpServer::seterrorcb(std::function<void (Connection *)>fn)
{
    errorcb_=fn;
}
void TcpServer::setslovemessagecb(std::function<void (Connection *,std::string)>fn)
{
    slovemessagecb_=fn;
}
void TcpServer::setsendCompletecb(std::function<void (Connection *)>fn)
{
    sendCompletecb_=fn;
}
void TcpServer::setepolltimeoutcb(std::function<void (EventLoop *)>fn)
{
    epolltimeoutcb_=fn;
}