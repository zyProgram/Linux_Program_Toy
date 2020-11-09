//
// Created by zhangyu on 2020/11/9.
//

#ifndef LINUX_DMS_CLRWLOCK_H
#define LINUX_DMS_CLRWLOCK_H
#include "pthread.h"
namespace zy{
    namespace thread{
        class CLRwLock {
        private:
            pthread_rwlock_t _lock;
        public:
            CLRwLock() ;

            ~CLRwLock() ;

            void Read_Lock();

            void Read_UnLock();

            void Write_Lock();

            void Write_UnLock();
        };
    }
}



#endif //LINUX_DMS_CLRWLOCK_H
