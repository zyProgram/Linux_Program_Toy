//
// Created by zhangyu on 2020/11/15.
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

void Reader(long row,bool multiFlag = true) {
    auto *p = new SLReaderMethodParam;
    p->row = row;
    p->callback = std::move([p](char *buf,int size) {
        std::string writeBuf;
        writeBuf += "multi thread read row:"+std::to_string(p->row)+",size:"+std::to_string(size)+" success";
        std::cout<<writeBuf<<std::endl;
        if(p->row>10000){
            std::abort();
        }
        zy::file::CLLogger::GetInstance()->WriteLog(writeBuf.c_str(),writeBuf.length());
    });
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
        delete p;
    }
}
int main(int argc,char **argv){
    srand((unsigned int)time(nullptr));
    struct  timeval  start;
    struct  timeval  end;

    gettimeofday(&start,NULL);
    if(argc < 2){
        std::cout<<"error input ,please ./exe read_row_num (option:thread_num test_read)"<<std::endl;
        return -1;
    }
    int totalRow = std::stoi(argv[1]);
    //创建表结构
    std::vector<CLObjectType> colums_type;
    colums_type.reserve(100);
    for(int i=0;i<100;i++){
        colums_type.push_back(CLObjectType::Int);
    }

    int thnum = (argc>2)?std::stoi(argv[2]):std::thread::hardware_concurrency();
    zy::dms::CLFileManager::InitInstance(1000,sizeof(int)*100,thnum);

    for (long i=0;i<totalRow;i++){
        long rownum= rand()%totalRow;
        std::cout<<"will read "<<rownum<<std::endl;
        Reader(rownum,thnum != 1);
    }
    gettimeofday(&end,NULL);
    std::cout<<"dms read row "<<totalRow<<" by "<<thnum<<" threads,consume "<<end.tv_sec*1000+end.tv_usec/1000
                                                                               -start.tv_sec*1000-start.tv_usec/1000<<"ms"<<std::endl;
    return 0;
}
