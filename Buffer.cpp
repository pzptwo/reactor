#include "Buffer.h"

Buffer::Buffer()
{

}
Buffer::~Buffer()
{

}

//这里是使用了stl的string
void Buffer::append(const char *data,size_t size)
{
    buf_.append(data,size);
}
//
const char* Buffer::data()
{
    return buf_.data();
}

size_t Buffer::size()
{
    return buf_.size();
}

void Buffer::clear()
{
    return buf_.clear();
}

void Buffer::erase(size_t pos,size_t n)
{
    buf_.erase(pos,n);
}