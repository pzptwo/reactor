#include "Connection.h"
#include "Channel.h"

Connection::Connection(EventLoop *loop, Socket *clientsock)
    : loop_(loop), clientsock_(clientsock) {
  clientchannel_ = new Channel(loop_, clientsock->fd());
  clientchannel_->setreadback(std::bind(&Connection::onMessage, this));
  clientchannel_->setcloseback(
      std::bind(&Connection::closecallback, this)); // 这里的是在Tcpserver回调
  clientchannel_->seterrorback(std::bind(&Connection::errorcallback, this));
  clientchannel_->setwriteback(std::bind(&Connection::writecallback, this));
  // clientchannel_->useet();
  clientchannel_->enablereading();
  // clientchannel.updatechannel(clientchannel);
}

Connection::~Connection() {
  delete clientchannel_;
  delete clientsock_; // 这里相当于我拿走了new的全部

  //打印日志
  printf("conn已被析构\n");
}

int Connection::fd() const { return clientsock_->fd(); }
// 返回ip_
std::string Connection::ip() { return clientsock_->ip(); }
// 返回port_
uint16_t Connection::port() { return clientsock_->port(); }

void Connection::closecallback() { closecallback_(shared_from_this()); }

void Connection::errorcallback() { errorcallback_(shared_from_this()); }

void Connection::setcloseback(std::function<void(spConnection)> fn) {
  closecallback_ = fn;
}

void Connection::seterrorback(std::function<void(spConnection)> fn) {
  errorcallback_ = fn;
}

void Connection::setslovecb(
    std::function<void(spConnection, std::string &)> fn) {
  slovemessagecallback_ = fn;
}

void Connection::setsendCompletecb(std::function<void(spConnection)> fn) {
  sendCompletecb_ = fn;
}

using namespace std;
void Connection::onMessage() {
  char buffer[1024];
  while (true) {
    bzero(buffer, sizeof(buffer)); // 这个函数与memset的区别
    ssize_t nread = read(fd(), buffer, sizeof(buffer)); // 这个函数的赋值？？？

    if (nread > 0) {
      // 这里看看要不要清空，inputbuffer_
      inputbuffer_.append(buffer, nread); // 现在的buffer没有数据
    }
    // 错误有好几种，有些需要排除
    else if (nread == -1 &&
             errno == EINTR) // 读取数据的时候被信号中断，继续读取。
    {
      continue;
    } else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
      // 这里开始读,逻辑是将客户端发过来进行读取
      while (true) {
        //////////////下面代码可以封装到buffer，还可以支持固定长度，指定报文长度，和分隔符等多种格式
        int len;
        // 上面已经接受了，现在是拷贝
        memcpy(&len, inputbuffer_.data(), sizeof(len));
        if (inputbuffer_.size() < len + 4)
          break;
        std::string message(inputbuffer_.data() + sizeof(len), len);
        inputbuffer_.erase(0, len + 4);
        /////////////////////////////////
        cout << "recv from client(eventfd=" << fd() << "): " << message.data()
             << endl;
        // 服务器对信息进行处理,数据的计算
        /*
        message="reply"+message;
        //发送模式为头加内容
        len=message.size();
        //这里用string，char都行
        //相当于进行增加报头，但是是拷贝构造？？？
        std::string tmpbuf((char *)&len,sizeof(len));
        tmpbuf.append(message);
        send(fd(),tmpbuf.data(),tmpbuf.size(),0);
        */
        slovemessagecallback_(shared_from_this(), message);
      }
      break;
    } else if (nread == 0) // 客户端连接已断开，和上面的重复了
    {
      closecallback(); // 关闭客户端的fd。
      break;
    }
  }
}

void Connection::sendto_ob(const char *data, size_t size) {
  // 这里不一样了，这里进缓冲区的是报文长度+内容
  outputbuffer_.appendwithhead(data, size);
  // 注册写事件
  clientchannel_->enablewriting();
}

void Connection::writecallback() {
  // 把outbuffer的数据发送出去
  int writen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);
  if (writen > 0)
    outputbuffer_.erase(0, writen);

  // 说明没有数据了，发送成功，取消写事件
  if (outputbuffer_.size() == 0)
    clientchannel_->disablewriting();

  sendCompletecb_(shared_from_this());
}