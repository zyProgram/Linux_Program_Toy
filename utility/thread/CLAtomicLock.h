//
// Created by zhangyu on 2020/10/23.
//

#ifndef LINUX_DMS_CLATOMICLOCK_H
#define LINUX_DMS_CLATOMICLOCK_H

#include <iostream>
#include "atomic"
#include "pthread.h"
namespace zy{
    namespace thread{
        class CLAtomicLock {
        private:
            std::atomic_bool _bool_lock;
            pthread_t _lock_thread_id;
        public:
            CLAtomicLock(const CLAtomicLock&&) = delete;
            CLAtomicLock(const CLAtomicLock&) = delete;
            CLAtomicLock(){
                _bool_lock.store(false);
            }
            void Lock(){
                bool expect = false;
                if(_bool_lock.compare_exchange_strong(expect, true)){
                    return;
                }
                _lock_thread_id = pthread_self();
            }
            void UnLock(){
                if(_lock_thread_id == pthread_self()){
                    bool expect = true;
                    _bool_lock.compare_exchange_strong(expect, false);
                }else{
                    std::cout<<"lock thread id is "<<_lock_thread_id<<",but unlock thread id is "<<pthread_self()<<std::endl;
                }
            }
        };
    }
}



#endif //LINUX_DMS_CLATOMICLOCK_H
