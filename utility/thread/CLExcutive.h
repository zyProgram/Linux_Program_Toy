//
// Created by zhangyu on 2020/10/19.
//

#ifndef CUTIL_CLEXCUTIVE_H
#define CUTIL_CLEXCUTIVE_H

#include <iostream>
#include "pthread.h"
namespace zy{
    namespace thread{
        class CLExcutiveAbstractFunc{
        protected:
            void *__runningContext;
        public:
            explicit CLExcutiveAbstractFunc(void *pContext):__runningContext(pContext){};
            virtual void RunFuncEntity() = 0;
            virtual ~CLExcutiveAbstractFunc() = default;
        };
        class CLAbstractExcutive {
        protected:
            CLExcutiveAbstractFunc *__func;
        public:
            explicit CLAbstractExcutive(CLExcutiveAbstractFunc *func):__func(func){};
            virtual ~CLAbstractExcutive(){
                delete __func;
            }
            virtual void run() = 0;
            virtual void wait() = 0;
        };

        class CLThreadExcutive: public CLAbstractExcutive{
        private:
            pthread_t _threadId;
            bool isCreated = false;
            static void* cThreadFunc(void *pContext);
        public:
            explicit CLThreadExcutive(CLExcutiveAbstractFunc *pFunc):CLAbstractExcutive(pFunc){}
            void run(){
                if(!isCreated){
                    if (0 != pthread_create(&_threadId, nullptr, cThreadFunc,this)){

                    }
                    isCreated = true;
                }

            }
            void wait(){
                if(isCreated){
                    pthread_join(_threadId, nullptr);
                }
            }
        };
        class TestFunc: public CLExcutiveAbstractFunc{
        public:
            TestFunc(void *pContext):CLExcutiveAbstractFunc(pContext){}
            void RunFuncEntity(){
                std::cout<<"testfunc:"<<*((long*)(__runningContext))<<std::endl;
            }
            ~TestFunc() = default;
        };
    }
}



#endif //CUTIL_CLEXCUTIVE_H
