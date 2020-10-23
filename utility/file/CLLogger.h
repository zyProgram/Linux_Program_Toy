//
// Created by zhangyu on 2020/10/21.
//

#ifndef LINUX_DMS_CLLOGGER_H
#define LINUX_DMS_CLLOGGER_H
#include "CLFile.h"
#include "string"
#include "../thread/CLLock.h"
namespace zy{
    namespace file{
        class CLLogger {
        private:
            static const std::string _sc_logger_name;

            CLFile *_logger_file = nullptr;
            bool _is_init = false;
            CLLogger(){
                try {
                    _logger_file = new CLFile(_sc_logger_name);
                } catch (std::string &errMsg) {
                    throw errMsg;
                }
            }
            ~CLLogger(){
                if(_logger_file!= nullptr){
                    delete _logger_file;
                }
            }
            static CLLogger* _s_logger;
            static thread::CLLock *_s_create_logger_lock;
        public:
            static CLLogger *GetInstance();
            static void OnExit();
            void WriteLog(const std::string &msg);
            void WriteLog(const char *msg, int total);
        };
    }
}



#endif //LINUX_DMS_CLLOGGER_H
