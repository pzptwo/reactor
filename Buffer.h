#pragma once
#include <cstddef>
#include <string>
#include <iostream>

class Buffer
{
    private:
        //定义一个容器，这里面回调用stl里面
        std::string buf_;
    public:
        Buffer();
        ~Buffer();
        void append(const char *data,size_t size);
        const char* data();//
        size_t size();
        void erase(size_t pos,size_t n);
        void clear();
};