//
// Created by zhangyu on 2020/10/21.
//
/**
 * Event实现更像信号量机制
 */
#ifndef CUTIL_CLEVENT_H
#define CUTIL_CLEVENT_H

#include <iostream>
#include "CLLock.h"
#include "CLConditionVar.h"
#include "CLLockGuard.h"
namespace zy{
    namespace thread{
        class CLEvent {
        private:
            CLLock _lock;
            CLConditionVar _condition;
            int _count;
        public:
            CLEvent(){
                _count = 0;
            }
            /**
             * Set可以被多次调用，形成多条通知消息
             * @return
             */
            void Set(){
                CLLockGuard lockGuard(&_lock);
                _count++;
                _condition.Notify_One();
            }
            /**
             * 有信号就处理，退化成信号量机制
             */
            void Wait(){
                CLLockGuard lockGuard(&_lock);
                while (_count == 0){
                    if(!_condition.Wait(_lock)){
                        std::cout<<"event wait fail"<<std::endl;
                        std::abort();
                    }
                }
                _count--;
            }
            ~CLEvent(){

            }
        };
    }
}



#endif //CUTIL_CLEVENT_H
