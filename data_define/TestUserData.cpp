//
// Created by zhangyu on 2020/10/24.
//
#include <memory>
#include <sys/time.h>
#include "CLObject.h"
#include "ctime"
#include "../app/CLFileManager.h"
#include "../utility/file/CLLogger.h"
using namespace zy::dms;
void storage(zy::dms::CLUserAttrVector *v,bool multiFlag=true) {
    int totalBytes = v->GetBufferSize();
    char *buf = new char[totalBytes];
    v->ToCharBuffer(buf);
    int row;
    auto *p = new SLWriterMethodParam;
    p->buf = buf;
    p->size = totalBytes;
    p->callback= [](int row,char *buf,int size) {
        std::string writeBuf;
        writeBuf += "multi thread write row:"+std::to_string(row)+",size:"+std::to_string(size)+" success";
        std::cout<<writeBuf<<std::endl;
        zy::file::CLLogger::GetInstance()->WriteLog(writeBuf.c_str(),writeBuf.length());
        delete []buf;
    };
    if(multiFlag){
        zy::dms::CLFileManager::GetInstance()->MultiWrite(p);
    }else{
        int futRow;
        zy::dms::CLFileManager::GetInstance()->Append(futRow,p->buf,p->size);
        std::string writeBuf;
        writeBuf += "no multi thread write row:"+std::to_string(futRow)+",size:"+std::to_string(p->size)+" success";
        std::cout<<writeBuf<<std::endl;
    }
}

void Reader(int row,bool multiFlag = true) {
    auto *p = new SLReaderMethodParam;
    p->row = row;
    p->callback = [&p](char *buf,int size) {
        std::string writeBuf;
        writeBuf += "multi thread read row:"+std::to_string(p->row)+",size:"+std::to_string(size)+" success";
        std::cout<<writeBuf<<std::endl;
        zy::file::CLLogger::GetInstance()->WriteLog(writeBuf.c_str(),writeBuf.length());
        delete []buf;
    };
    if(multiFlag){
        zy::dms::CLFileManager::GetInstance()->MultiRead(p);
    }else{
        int size = CLFileManager::GetRowSize();
        char buf[size];
        if(zy::dms::CLFileManager::GetInstance()->Read(row,buf,CLFileManager::GetRowSize())){
            std::string writeBuf;
            writeBuf += "no multi thread write row:"+std::to_string(row)+",size:"+std::to_string(size)+" success";
            std::cout<<writeBuf<<std::endl;
        }else{
            std::cout<<"read "<<row<<" fail"<<std::endl;
        }
    }
}
int main(int argc,char **argv){
    srand((unsigned int)time(nullptr));
    struct  timeval  start;
    struct  timeval  end;

    gettimeofday(&start,NULL);
    if(argc < 2){
        std::cout<<"error input ,please ./exe write_row_num (option:thread_num test_read)"<<std::endl;
        return -1;
    }
    int totalRow = std::stoi(argv[1]);
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
    int thnum = (argc>2)?std::stoi(argv[2]):std::thread::hardware_concurrency();
    zy::dms::CLFileManager::InitInstance(1000,sizeof(int)*100,thnum);
    for(int i=0;i<totalRow;i++){
        auto &v = userInfo[i];
        for(int j=1;j<=100;j++){
            auto *intObject = factory.CreateObject(rand()%100);
            v->PushBack(intObject);
        }
        storage(v.get(),thnum!=1);
    }
    if(argc > 3){
        int readrow = atoi(argv[3]);
        int size = 400;
        char buf[size];
        if(zy::dms::CLFileManager::GetInstance()->Read(readrow,buf,size)){
            CLUserAttrVector desVec(colums_type);
            desVec.FromCharBuffer(buf);
            desVec.Print();
        }else{
            std::cout<<"read row="<<readrow<<" error may not exist"<<std::endl;
        }
    }
    gettimeofday(&end,NULL);
    std::cout<<"dms write row "<<totalRow<<" by "<<thnum<<" threads,consume "<<end.tv_sec*1000+end.tv_usec/1000
    -start.tv_sec*1000-start.tv_usec/1000<<"ms"<<std::endl;
    return 0;
}
