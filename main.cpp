#include <iostream>
#include "utility/file/CLLogger.h"
#include "utility/thread/ZyThreadExtern.h"
#include "stdlib.h"

#include "app/CLTableFile.h"
#include "app/CLFileManager.h"
using namespace zy::file;
using namespace zy::thread;
using namespace zy::dms;
class TestLoggerFunc: public CLExcutiveAbstractFunc{
public:
    TestLoggerFunc(void *pContext):CLExcutiveAbstractFunc(pContext){}
    void RunFuncEntity(){
        int count = rand()%200;
        char id = *((char*)(__runningContext));
        for (int i =0 ;i<count;i++){
            int total = rand()%5000;
            char buf[total + 1];
            char ch = 'a'+rand()%26;
            for(int i = 0;i<total;i++){
                buf[i] = ch;
            }
            try{
                CLLogger::GetInstance()->WriteLog(buf, total + 1);
            } catch (std::string &errMsg) {
                std::cout<<errMsg<<std::endl;
            }

            std::cout<<"i am :"<<id<<",the "<<i<<" time ,i will write "<<total + 1 <<"(include \\n) bytes"<<std::endl;
        }
    }
    ~TestLoggerFunc() = default;
};

class TestWriterFunc: public CLExcutiveAbstractFunc{
public:
    TestWriterFunc(void *pContext):CLExcutiveAbstractFunc(pContext){}
    void RunFuncEntity(){
        int count = 1000;
        CLAbstractTableFile *f = static_cast<CLAbstractTableFile*>(__runningContext);
        int bufsize = 110;
        char buf[bufsize];
        for(int i=0;i<count;i++){
            for (int j =0 ;j<bufsize;j++){
                buf[j] = rand()%26+'a';
            }
            f->WriteRow(buf,bufsize);
//            std::cout<<"the "<<i<<" time ,i write finished"<<std::endl;
        }
        std::cout<<"i write finished"<<std::endl;
    }
    ~TestWriterFunc() = default;
};

class TestReaderFunc: public CLExcutiveAbstractFunc{
public:
    TestReaderFunc(void *pContext):CLExcutiveAbstractFunc(pContext){}
    void RunFuncEntity(){
        int rows = 10;
        int bufsize = 110;
        CLAbstractTableFile *f = static_cast<CLAbstractTableFile*>(__runningContext);
        char buf[bufsize];
        std::cout<<"test reader"<<std::endl;
        for (int i =1 ;i<=rows;i++){
            std::cout<<i<<":";
            if(f->ReadRow(i,buf,bufsize)){
                for (int j =0;j<bufsize;j++){
                    std::cout<<buf[j];
                }
                std::cout<<std::endl;
            }
        }
    }
    ~TestReaderFunc() = default;
};
//use logger to test file
//int main(){
//    srand(unsigned (time(nullptr)));
//    int thread_num = 1;
//    CLAbstractExcutive *th[thread_num];
//    char paras[thread_num];
//    for(char i=0;i<thread_num;i++){
//        paras[i] = i;
//        CLExcutiveAbstractFunc *func =new TestLoggerFunc(paras+i);
//        th[i] = new CLThreadExcutive(func);
//    }
//    for(char i=0;i<thread_num;i++){
//        th[i]->run();
//    }
//    for(char i=0;i<thread_num;i++){
//        th[i]->wait();
//    }
//}
//test write and read
//int main() {
//    srand(unsigned (time(nullptr)));
//
//    CLAbstractTableFile *tabelfile = new CLTableFile("test_read.txt",110);
//    CLExcutiveAbstractFunc *rf = new TestReaderFunc(tabelfile);
//    CLExcutiveAbstractFunc *wf = new TestWriterFunc(tabelfile);
//    CLAbstractExcutive *wth = new CLThreadExcutive(wf);
//    CLAbstractExcutive *rth = new CLThreadExcutive(rf);
//
//    wth->run();
//    wth->wait();
//    rth->run();
//    rth->wait();
//    delete tabelfile;
//    return 0;
//}

//test app 's write and read
int main() {
    srand(unsigned (time(nullptr)));
    int bufsize = 100;
    CLFileManager::InitInstance(90000,bufsize);
    auto *instance = CLFileManager::GetInstance();
    int count = 90000;
    char buf[bufsize];
    for(int i=0;i<count;i++){
        for (int j =0 ;j<bufsize;j++){
            buf[j] = rand()%26+'a';
        }
        instance->Append(buf,bufsize);
    }
    std::cout<<"write finished"<<std::endl;

    std::cout<<"test reader"<<std::endl;
    for (int i =89900;i<=90000;i++){
        std::cout<<i<<":";
        if(instance->Read(i,buf,bufsize)){
            for (int j =0;j<bufsize;j++){
                std::cout<<buf[j];
            }
            std::cout<<std::endl;
        }
    }
    return 0;
}
