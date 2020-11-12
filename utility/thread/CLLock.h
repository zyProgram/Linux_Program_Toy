//
// Created by zhangyu on 2020/10/19.
//

#ifndef CUTIL_CLLOCK_H
#define CUTIL_CLLOCK_H

#include <pthread.h>
#include "exception"
#include <cerrno>
#include <cstring>
#include "string"
namespace zy {
    namespace thread {
        class CLLock {
        private:
            bool _locked = false;
            pthread_mutex_t _mutex;
        public:
            //为了封装条件变量，_mutex必须暴露出去
            pthread_mutex_t GetMutex();
            pthread_mutex_t *GetMutexPtr();
            CLLock() {
                int res = pthread_mutex_init(&_mutex, nullptr);
                if (res) {
                    std::string errMsg = "pthread_mutex_init error:";
                    errMsg += strerror(errno);
                    throw errMsg;
                }
            }

            ~CLLock() {
                 pthread_mutex_destroy(&_mutex);
            }

            void Lock() {
                if (0 != pthread_mutex_lock(&_mutex)) {
                    std::string errMsg = "pthread_mutex_lock error:";
                    errMsg += strerror(errno);
                    throw errMsg;
                }
                _locked = true;
            }

            void UnLock() {
                if (_locked) {
                    if (0 != pthread_mutex_unlock(&_mutex)) {
                        std::string errMsg = "pthread_mutex_unlock error:";
                        errMsg += strerror(errno);
                        throw errMsg;
                    }
                } else {
                    std::string errMsg = "pthread_mutex_unlock error:";
                    errMsg += strerror(errno);
                    throw errMsg;
                }
            }
        };
    }
}

#endif //CUTIL_CLLOCK_H
