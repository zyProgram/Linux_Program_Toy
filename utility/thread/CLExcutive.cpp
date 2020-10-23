//
// Created by zhangyu on 2020/10/19.
//

#include "CLExcutive.h"

void *zy::thread::CLThreadExcutive::cThreadFunc(void *pContext) {
    CLThreadExcutive* pThread = (CLThreadExcutive*)(pContext);
    pThread->__func->RunFuncEntity();
    return nullptr;
}
