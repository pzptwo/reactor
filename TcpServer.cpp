#include "TcpServer.h"
#include "Acceptor.h"
#include "Connection.h"
#include <functional>
#include <iostream>
using namespace std;


TcpServer::TcpServer(const std::string ip,const uint16_t port)
{
    acceptor_=new Acceptor(&loop_,ip,port);
    //刚刚启动，还没有socket所以不能传clientsock
    acceptor_->setnewConnectioncb(std::bind(&TcpServer::newConnection,this,std::placeholders::_1));
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
    cout << "new Connection: fd=" << clientsock->fd()
            << ", ip=" << clientsock->ip()
            << ", port=" <<  clientsock->port()<< endl;
    conns_[conn->fd()]=conn;
}

void TcpServer::closecallback(Connection * conn)
{
    printf("client(eventfd=%d) disconnected.\n",conn->fd());
    //close(fd()); 
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorcallback(Connection* conn)
{
    printf("client(eventfd=%d) error.\n",conn->fd());
    //close(conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::slovemessage(Connection* conn,std::string message)
{
    message="reply"+message;
    //发送模式为头加内容
    int len=message.size();
    //这里用string，char都行
    //相当于进行增加报头，但是是拷贝构造？？？
    std::string tmpbuf((char *)&len,sizeof(len));
    tmpbuf.append(message);
    //send(conn->fd(),tmpbuf.data(),tmpbuf.size(),0);   //对于Socket，这里有隐藏的bug
    conn->sendto_ob(tmpbuf.data(),tmpbuf.size());   //作用是
}