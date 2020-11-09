//
// Created by zhangyu on 2020/11/9.
//

#include "CLRwLock.h"
#include "string"
#include "cstring"
zy::thread::CLRwLock::CLRwLock() {
    int res = pthread_rwlock_init(&_lock, nullptr);
    if (res) {
        std::string errMsg = "pthread_mutex_init error:";
        errMsg += strerror(errno);
        throw errMsg;
    }
}

zy::thread::CLRwLock::~CLRwLock() {
    pthread_rwlock_destroy(&_lock);
}

void zy::thread::CLRwLock::Read_Lock() {
    pthread_rwlock_rdlock(&_lock);
}

void zy::thread::CLRwLock::Read_UnLock() {
    pthread_rwlock_unlock(&_lock);
}

void zy::thread::CLRwLock::Write_Lock() {
    pthread_rwlock_wrlock(&_lock);
}

void zy::thread::CLRwLock::Write_UnLock() {
    pthread_rwlock_unlock(&_lock);
}


