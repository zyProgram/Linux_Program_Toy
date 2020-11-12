//
// Created by zhangyu on 2020/10/24.
//
#include <memory>
#include "CLObject.h"
#include "ctime"
#include "../app/CLFileManager.h"
using namespace zy::dms;
void storage(zy::dms::CLUserAttrVector *v) {
    int totalBytes = v->GetBufferSize();
    char *buf = new char[totalBytes];
    v->ToCharBuffer(buf);
    int row;
    SLWriterMethodParam *p  =new SLWriterMethodParam;
    p->buf = buf;
    p->size = totalBytes;
    p->callback= [&p](int row,char *buf,int size) {
        std::cout<<"row:"<<row<<",size:"<<size<<std::endl;
        delete []buf;
        delete p;
    };
    zy::dms::CLFileManager::GetInstance()->MultiWrite(p);
}
int main(){
    srand((unsigned int)time(nullptr));
    int totalRow = 1000;
    //construct 100 attribute type of one row
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
        if(i==1){
            v->Print();
        }
        storage(v.get());
    }
    int size = 400;
    char buf[size];
    zy::dms::CLFileManager::GetInstance()->Read(2,buf,size);
    CLUserAttrVector desVec(colums_type);
    desVec.FromCharBuffer(buf);
//    desVec.Print();
    return 0;
}
