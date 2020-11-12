//
// Created by zhangyu on 2020/11/11.
//

#ifndef LINUX_DMS_CLTHREADPOOL_H
#define LINUX_DMS_CLTHREADPOOL_H

#include <atomic>
#include "queue"
#include "CLExcutive.h"
#include "CLLock.h"
#include "CLEvent.h"
#include "CLLockGuard.h"
namespace zy{
    namespace thread{
        class CLThreadPool {
        private:
            std::vector<CLThreadExcutive *> _worker_queue;
            std::queue<CLExcutiveAbstractFunc*> _task_queue;

            CLLock _task_submit_get_lock;
            int _submit_task_num = 0;
            bool _started = false;
        public:
            CLEvent _has_task_event; //have to expose to single thread
            explicit CLThreadPool(int taskNum);
            void Start();
            bool Submit(CLExcutiveAbstractFunc *func);
            bool GetFunc(CLExcutiveAbstractFunc *func);
            bool ShutDown();
            ~CLThreadPool();
        };

        class CLRwFunc: public CLExcutiveAbstractFunc{
        private:
            std::atomic<bool> _running;
            int _total_task_num = 0;
            int _id;

        public:
            explicit CLRwFunc(int id,void *pContext):CLExcutiveAbstractFunc(pContext){
                _running.store(true);
                _id = id;
            };
            virtual void RunFuncEntity() {
                CLThreadPool *pool = (CLThreadPool *)(__runningContext);
                while (_running){
                    pool->_has_task_event.Wait();
                    CLExcutiveAbstractFunc *func;
                    if(pool->GetFunc(func)){
                        func->RunFuncEntity();
                        _total_task_num ++;
                    }else{
                        std::cout<<"worker "<<_id<<" is notifyed,but can not find any task,some logic error"<<std::endl;
                    }
                }
                std::cout << "thread " << _id << " quit,total handle work number:" << _total_task_num << std::endl;
            }
            virtual ~CLRwFunc() {
                _running.store(false);
            }
        };

    }
}



#endif //LINUX_DMS_CLTHREADPOOL_H
