//
// Created by zhangyu on 2020/10/19.
//

#include "CLLock.h"

pthread_mutex_t zy::thread::CLLock::GetMutex() {
    return _mutex;
}
pthread_mutex_t* zy::thread::CLLock::GetMutexPtr() {
    return &_mutex;
}
