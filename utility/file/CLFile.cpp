//
// Created by zhangyu on 2020/10/21.
//

#include "CLFile.h"

bool zy::file::CLFile::Write(const char *buffer, int total) {
    {
        int remain = _GetReamin();
        if(total < remain){
            memcpy(_buffer+_used,buffer,total);
            _used += total;
        }else{
            int offset = remain;
            //先dump 当前buffer到磁盘
            memcpy(_buffer+_used,buffer,remain);
            _NormalWrite(_buffer,MAX_FILE_BUF_SIZE);
            int left = total - remain;
            while (left > MAX_FILE_BUF_SIZE){
                memcpy(_buffer,buffer+offset,MAX_FILE_BUF_SIZE);
                _used = MAX_FILE_BUF_SIZE;
                _NormalWrite(_buffer,_used);
                left -= MAX_FILE_BUF_SIZE;
                offset += MAX_FILE_BUF_SIZE;
                memset(_buffer,0,MAX_FILE_BUF_SIZE);
                _used = 0;
            }
            memcpy(_buffer+_used,buffer+offset, left);
            _used += left;
        }
        return true;
    }
}

bool zy::file::CLFile::_NormalWrite(const char *buffer, ssize_t size) {
    ssize_t bytes = ::write(_open_file_handle, buffer, size);
    if(bytes < 0){
        std::cout<<"write file "<<_filename<< " failed:"<<strerror(errno)<<std::endl;
        //TODO:错误处理
    }
    memset(_buffer,0,MAX_FILE_BUF_SIZE);
    _used = 0;
    return true;
}

void zy::file::CLFile::Flush() {
    if(_open_file_handle < 0){
        return ;
    }
    _NormalWrite(_buffer,_used);
    return ;
}

void zy::file::CLFile::Close() {
    if(_open_file_handle > 0){
        Flush();
        //::fsync(_open_file_handle);
        ::close(_open_file_handle);
    }
}

bool zy::file::CLFile::Read(long offset, char *buffer, int total) {
    off_t off = lseek(_open_file_handle,offset,SEEK_SET);
    if (read(_open_file_handle,buffer,total) < 0){
        std::cout<<"error read:"<<strerror(errno)<<std::endl;
        return false;
    }
    return true;
}

void zy::file::CLFile::Clear() {
    ftruncate(_open_file_handle,0);
    lseek(_open_file_handle,0,SEEK_SET);
}

bool zy::file::CLFile::ReadAll(char *buffer, int total, ssize_t &realCount) {
    off_t off = lseek(_open_file_handle,0,SEEK_SET);
    realCount = read(_open_file_handle,buffer,total);
    if (realCount < 0){
        std::cout<<"error read:"<<strerror(errno)<<std::endl;
        return false;
    }
    return true;
}



