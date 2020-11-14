//
// Created by zhangyu on 2020/10/21.
//

#ifndef LINUX_DMS_CLTABLEFILE_H
#define LINUX_DMS_CLTABLEFILE_H

#include <vector>
#include "file/CLFile.h"
#include "cassert"
namespace zy{
    namespace dms{
        /***
         * 用于GFS的文件系统客户端，同样可以采用继承CLAbstractTableFile的方式
         * class CLGFSclientFile:public CLAbstractTableFile{
         * private:
         *     std::unordered_map<> _ip_table;
         * public:
         *     void write(){
         *        implement;
         *     }
         *     void read(){
         *        implement;
         *     }
         * }
         */
        class CLAbstractTableFile{
        public:
            CLAbstractTableFile() = default;
            virtual bool WriteRow(const char *buffer, int size) = 0;
            virtual bool WriteRow(const std::string &msg) = 0;
//            virtual bool WriteRows(const std::vector<std::string> &msg) = 0;
            virtual bool ReadRow(int rowLine, char *buffer, int maxsize) = 0;
            virtual ~CLAbstractTableFile() = default;
            virtual void Flush() = 0;
        };
        class CLTableFile: public CLAbstractTableFile{
        private:
            static int _s_header_size;
            static int _s_suffix_size;
            int _row_size;
            file::CLFile *_table_file = nullptr;
            std::string _file_name;
            int _cur_row = 0;
            bool _WriteSuffix(){
#ifdef _WIN32
                return _table_file->Write("\n",CLTableFile::_s_suffix_size-1);
#else
                return _table_file->Write("\n",CLTableFile::_s_suffix_size);
#endif
            }
        public:
            CLTableFile(const std::string &name,int rowSize){
                _file_name = name;
                _row_size = _s_header_size + rowSize + _s_suffix_size;
                try {
                    _table_file = new file::CLFile(name);
                } catch (std::string &errMsg) {
                    throw errMsg;
                }
            }
            ~CLTableFile() override {
                if(_table_file != nullptr){
                    delete _table_file;
                }
            }
            void Flush(){
                _table_file->Flush();
            }
            /**
             * 写入一行数据，加上前缀row 和 末尾\n
             * @param msg
             * @return true or false
             */
            bool WriteRow(const std::string &msg) override{
                int id = _cur_row+1;
                char headbuf[CLTableFile::_s_header_size];
                memcpy(headbuf,&id,CLTableFile::_s_header_size);
                _table_file->Write(headbuf,CLTableFile::_s_header_size);
                _table_file->Write(msg.c_str(),msg.length());
                _WriteSuffix();
                _cur_row++;
                return true;
            }
            bool WriteRow(const char *buffer, int size) override{
                int id = _cur_row+1;
                char headbuf[CLTableFile::_s_header_size];
                memcpy(headbuf,&id,CLTableFile::_s_header_size);
                //TODO:write fail
                _table_file->Write(headbuf,CLTableFile::_s_header_size);
                _table_file->Write(buffer,size);
                _WriteSuffix();
                _cur_row++;
                return true;
            }
            bool ReadRow(int rowLine, char *buffer, int size) override{
                if(size < _row_size - _s_header_size - _s_suffix_size){
                    std::cout<<"invalid read parameter : "<<size<< " < "<<_row_size - _s_header_size - _s_suffix_size <<std::endl;
                    std::abort();
                    return false;
                }
                int offset = _row_size * (((rowLine-1) > 0) ? rowLine-1:0);
                char lowBuf[size + CLTableFile::_s_header_size+CLTableFile::_s_suffix_size];
                bool ret = _table_file->Read(offset,lowBuf , _row_size - CLTableFile::_s_header_size);

                if(ret){
                    memcpy(buffer, lowBuf + CLTableFile::_s_header_size,size);
                }
                return ret;
            }
        };

    }
}



#endif //LINUX_DMS_CLTABLEFILE_H
