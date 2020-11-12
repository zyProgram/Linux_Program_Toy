//
// Created by zhangyu on 2020/10/20.
//

#ifndef CUTIL_CLCONDITIONVAR_H
#define CUTIL_CLCONDITIONVAR_H
#include "thread"
#include "cstring"
#include "CLLock.h"
namespace zy{
    namespace thread{
        class CLConditionVar {
        private:
            pthread_cond_t _condition_id;
            bool isInit;
        public:
            CLConditionVar(){
                if(0 != pthread_cond_init(&_condition_id, nullptr)){
                    throw std::string("pthread_cond_init error:")+strerror(errno);
                }
            }
            ~CLConditionVar(){
                pthread_cond_destroy(&_condition_id);
            }
            bool Notify_One(){
                return pthread_cond_signal(&_condition_id) == 0;
            }
            bool Notify_All(){
                return 0 == pthread_cond_broadcast(&_condition_id);
            }

            bool Wait(CLLock &lock){
                return 0 == pthread_cond_wait(&_condition_id, lock.GetMutexPtr());
            }
        };
    }
}



#endif //CUTIL_CLCONDITIONVAR_H
