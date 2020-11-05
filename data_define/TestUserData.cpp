//
// Created by zhangyu on 2020/10/24.
//
#include "CLObject.h"
#include "ctime"
using namespace zy::dms;
int main(){
    zy::dms::CLUserAttrVector v;
    std::string typestr[5] = {"Char","Int","Short","Double","Float"};
    srand((unsigned int)time(nullptr));
    CLObjectProduceFactory factory;
    char ch = 'a';
    for(int i=1;i<=20;i++){
        int r = rand()%2;
        if(r){
            auto *charObject = factory.CreateObject(ch++);
            v.PushBack(charObject);
        }else{
            auto *doubleObject = factory.CreateObject(2.2f);
            v.PushBack(doubleObject);
        }
    }
    v.Print();
    char buf[v.GetBufferSize()];
    v.ToCharBuffer(buf);
    zy::dms::CLUserAttrVector v2;
    v2.FromCharBuffer(buf);
//    delete charObject;
//    delete doubleObject;
    return 0;
}
