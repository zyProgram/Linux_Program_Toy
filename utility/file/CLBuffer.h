//
// Created by zhangyu on 2020/10/21.
//

#ifndef LINUX_DMS_CLBUFFER_H
#define LINUX_DMS_CLBUFFER_H


#include <iostream>
#include "string"
#include "cstring"
#define MAX_FILE_BUF_SIZE 4096
namespace zy{
    namespace file{
        class CLBuffer{
        private:
            int _buffer_size;
            char *_buffer = nullptr;
            int _used = 0;
        public:
            CLBuffer(int bufsize = MAX_FILE_BUF_SIZE){
                _buffer = new char[bufsize];
                _used = 0;
                //TODO: bad_alloc 处理
            }
            void Bzero(){
                if(_buffer != nullptr){
                    memset(_buffer,0,_buffer_size);
                }
            }
            ~CLBuffer(){
                if(_buffer != nullptr){
                    delete _buffer;
                }
            }
        };
    }
}


#endif //LINUX_DMS_CLBUFFER_H
