//
// Created by zhangyu on 2020/10/21.
//

#include "CLLogger.h"
#include "time.h"
zy::thread::CLLock *zy::file::CLLogger::_s_create_logger_lock = new zy::thread::CLLock;
zy::file::CLLogger *zy::file::CLLogger::_s_logger = nullptr;
const std::string zy::file::CLLogger::_sc_logger_name = "linux_dms_logger.txt";
zy::file::CLLogger *zy::file::CLLogger::GetInstance() {
    if(_s_logger != nullptr){
        return _s_logger;
    }
    _s_create_logger_lock->Lock();
    if(_s_logger == nullptr){
        try{
            _s_logger = new CLLogger();
        } catch (std::string &errMsg) {
            _s_create_logger_lock->UnLock();
            throw errMsg;
        }
    }
    _s_create_logger_lock->UnLock();
    atexit(CLLogger::OnExit);
    return _s_logger;
}

void zy::file::CLLogger::OnExit() {
    if(_s_create_logger_lock != nullptr){
        delete _s_create_logger_lock;
    }
    if(_s_logger != nullptr){
        delete _s_logger;
    }
}

void zy::file::CLLogger::WriteLog(const std::string &msg) {
    _WritePrefix();
    _logger_file->Write(msg.c_str(),msg.length());
    _WriteSuffix();
}

void zy::file::CLLogger::WriteLog(const char *msg, int total) {
    _WritePrefix();
    _logger_file->Write(msg,total);
    _WriteSuffix();
}

void zy::file::CLLogger::_WriteSuffix() {
    _logger_file->Write("\n",1);
}
void zy::file::CLLogger::_WritePrefix() {
    time_t timep;
    time(&timep);
    std::string buf(asctime(gmtime(&timep)));
    buf.back() = ':'; //替换回车为冒号
    _logger_file->Write(buf.c_str(),buf.length());
}