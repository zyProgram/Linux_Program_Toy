//
// Created by zhangyu on 2020/11/11.
//

#include "CLThreadPool.h"
namespace zy{
    namespace thread{
        CLThreadPool::CLThreadPool(int taskNum) {
            for(int i=1;i<=taskNum;i++){
                try {
                    auto *loopWork = new CLRwFunc(i,this);
                    auto *worker = new CLThreadExcutive(loopWork);
                    _worker_queue.push_back(worker);
                } catch (std::exception &e) {
                    std::cout<<"error init task queue of thread"<<i<<std::endl;
                    std::abort();
                }
            }
        }

        bool CLThreadPool::Submit(CLExcutiveAbstractFunc *func) {
            if(!_started){
                std::cout<<"submit a task on stoped thread pool"<<std::endl;
                return false;
            }
            CLLockGuard lockGuard(&_task_submit_get_lock);
            _task_queue.push(func);
            _has_task_condition_var.Notify_One();
            _submit_task_num++;
            return true;
        }

        bool CLThreadPool::GetFunc(CLExcutiveAbstractFunc **func) {
            if(_task_queue.empty()){
                func = nullptr;
                std::cout<<"hard to be here theortily,when shutdown or ctrl-c"<<std::endl;
                return false;
            }
            *func = _task_queue.front();
            _task_queue.pop();
            return true;
        }

        CLThreadPool::~CLThreadPool() {
            ShutDown();
        }

        void CLThreadPool::Start() {
            for(int i=0;i<_worker_queue.size();i++){
                _worker_queue[i]->run();
            }
            _started = true;
        }

        bool CLThreadPool::ShutDown() {
            for(int i=0;i<_worker_queue.size();i++){
                delete _worker_queue[i];
            }
            //delete 会让所有线程的while(true)中的isruning进入false状态，然后重新唤起所有处于wait的线程进行退出
            _has_task_condition_var.Notify_All();
            return true;
        }
    }
}


