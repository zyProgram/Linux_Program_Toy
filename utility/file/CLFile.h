//
// Created by zhangyu on 2020/10/21.
//

#ifndef LINUX_DMS_CLFILE_H
#define LINUX_DMS_CLFILE_H

#include <iostream>
#include "string"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "cstring"
#define MAX_FILE_BUF_SIZE 4096
namespace zy{
    namespace file{
        class CLFile {
        private:
            int _open_file_handle = -1;
            char _buffer[MAX_FILE_BUF_SIZE];             //暂时采用临时缓存
            int _used = 0;
            std::string _filename;
            int ceiling = 102400;
            int _GetReamin(){
                return MAX_FILE_BUF_SIZE - _used;
            }
            int _AddFile(){
                return MAX_FILE_BUF_SIZE - _used;
            }
            bool _NormalWrite(const char*buffer,ssize_t size);
        public:
            CLFile(const std::string &filename){
                _open_file_handle = ::open(filename.c_str(),
                                             O_RDWR|O_CREAT|O_APPEND,
                                             S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH);
                if(_open_file_handle<0){
                    throw std::string("open ") + filename + " failed with num " + std::to_string(errno)+ ":" + strerror(errno);
                }
                _filename = filename;
            }
            bool IsEmpty(){
                off_t off = lseek(_open_file_handle,0,SEEK_SET);
                char ch = EOF;
                Read(0, &ch, 1);
                return ch == EOF;
            }
            bool Write(const char *buffer,int total);
            bool Read(long offset, char *buffer, int total);
            bool ReadAll(char *buffer, int total,ssize_t &realCount);
            void Flush();
            void Close();
            void Clear();
            ~CLFile(){
                Close();
            }
        };
    }
}



#endif //LINUX_DMS_CLFILE_H
