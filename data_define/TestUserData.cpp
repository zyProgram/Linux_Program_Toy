//
// Created by zhangyu on 2020/10/24.
//
#include <memory>
#include "CLObject.h"
#include "ctime"
#include "../app/CLFileManager.h"
#include "../utility/file/CLLogger.h"
using namespace zy::dms;
void storage(zy::dms::CLUserAttrVector *v) {
    int totalBytes = v->GetBufferSize();
    char *buf = new char[totalBytes];
    v->ToCharBuffer(buf);
    int row;
    auto *p = new SLWriterMethodParam;
    p->buf = buf;
    p->size = totalBytes;
    p->callback= [](int row,char *buf,int size) {
        std::string writeBuf;
        writeBuf += "write row:"+std::to_string(row)+",size:"+std::to_string(size)+" success";
        std::cout<<writeBuf<<std::endl;
        zy::file::CLLogger::GetInstance()->WriteLog(writeBuf.c_str(),writeBuf.length());
        delete []buf;
    };
    zy::dms::CLFileManager::GetInstance()->MultiWrite(p);
}
int main(){
    srand((unsigned int)time(nullptr));
    int totalRow = 1000;
    //创建表结构
    std::vector<CLObjectType> colums_type;
    colums_type.reserve(100);
    for(int i=0;i<100;i++){
        colums_type.push_back(CLObjectType::Int);
    }

    std::unique_ptr<zy::dms::CLUserAttrVector> userInfo[totalRow];
    for(int i=0;i<totalRow;i++){
        userInfo[i].reset(new CLUserAttrVector(colums_type));
    }
    CLObjectProduceFactory factory;

    zy::dms::CLFileManager::InitInstance(1000,sizeof(int)*100);
    for(int i=0;i<1000;i++){
        auto &v = userInfo[i];
        for(int j=1;j<=100;j++){
            auto *intObject = factory.CreateObject(rand()%100);
            v->PushBack(intObject);
        }
        storage(v.get());
    }
    int size = 400;
    char buf[size];
    zy::dms::CLFileManager::GetInstance()->Read(1994,buf,size);
    CLUserAttrVector desVec(colums_type);
    desVec.FromCharBuffer(buf);
    desVec.Print();
    return 0;
}
