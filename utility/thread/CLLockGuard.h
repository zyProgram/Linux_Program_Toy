//
// Created by zhangyu on 2020/10/21.
//

#ifndef CUTIL_CLLOCKGUARD_H
#define CUTIL_CLLOCKGUARD_H
#include "CLLock.h"
namespace zy{
    namespace thread{
        class CLLockGuard {
        private:
            CLLock *_lock;
        public:
            CLLockGuard(CLLock *lock):_lock(lock){
                try {
                    _lock->Lock();
                } catch (std::string &errMsg) {
                    throw errMsg;
                }
            }
            ~CLLockGuard(){
                _lock->UnLock();
            }
        };
    }
}



#endif //CUTIL_CLLOCKGUARD_H
