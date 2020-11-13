//
// Created by zhangyu on 2020/10/22.
//

#ifndef LINUX_DMS_CLFILEMANAGER_H
#define LINUX_DMS_CLFILEMANAGER_H

#include <thread/CLLock.h>
#include "unordered_map"
#include "CLTableFile.h"
#include "thread/CLRwLock.h"
#include "CLTableConfigure.h"
#include "thread/CLThreadPool.h"
#include "thread/CLExcutive.h"
#include "CLMutilReadWriter.h"
namespace zy{
    namespace dms{
        struct SLReaderMethodParam{
            int row;
            std::function<void(char *buf,int size)> callback;
        };
        struct SLWriterMethodParam{
            char *buf;
            int size;
            std::function<void(int row,char *buf,int size)> callback;
        };

        class CLFileManager {
        private:
            static std::string _s_meta_data_file_prefix;
            static int _s_max_rows_for_per_file;
            static std::string _s_dms_prefix;
            static std::string _s_dms_suffix;
            static thread::CLLock _s_create_instance_lock;
            static CLFileManager *_s_instance;
            static int _s_row_size;
            friend class CLTableConfigure;
            CLTableConfigure _configure;
            CLTableMetaData _meta_file;
            /**
             * 嵌套类实现 内存回收，通过静态的嵌套类 析构来 delete instance，也可以用atexist()
             */
            class CLManagerGC{
            public:
                CLManagerGC() = default;
                ~CLManagerGC(){
                    if(_s_instance!= nullptr){
                        delete _s_instance;
                    }
                }
            };
            static CLManagerGC _s_gc;

            int _max_file_index = 10;
            std::unordered_map<std::string ,CLAbstractTableFile *> _file_manager;
            long long _cur_total_rows;
            std::unordered_map<std::string, int> _manage_file_rows_map;
            std::string _top_dir;
            thread::CLRwLock _rwLock;
            thread::CLThreadPool *_worker_pool;

            bool _EqualMaxRow(){
                return 0 ==  _cur_total_rows % _s_max_rows_for_per_file;
            }
            std::string _ConstructFileName(int index){
                return _s_dms_prefix+std::to_string(index)+_s_dms_suffix;
            }
            bool _FindCurCLFile(CLAbstractTableFile **pFile);
            bool _FindCLFile(int row,CLAbstractTableFile **pFile);

            bool _ExtendLocalCLFile();
        public:
            CLFileManager(int num=std::thread::hardware_concurrency())
            {
                //TODO 处理文件夹
                _worker_pool = new thread::CLThreadPool(num);
                _worker_pool->Start();
                _cur_total_rows = 0;
                FromStorage();
                for (int i=1;i<=_max_file_index;i++){
                    std::string filename =_ConstructFileName(i);
                    try {
                        _file_manager.emplace(filename,new CLTableFile(filename,CLFileManager::_s_row_size));
                        _manage_file_rows_map.emplace(filename,0);
                    } catch (std::string &errMsg) {
                        std::cout<<"new file manager catch:"<<errMsg<<std::endl;
                    } catch (std::exception &e) {
                        std::cout<<"new file manager exception:"<<e.what()<<std::endl;
                    }
                }
            }
            ~CLFileManager(){
                for(int i=0;i<_max_file_index;i++){
                    std::string filename = _ConstructFileName(i);
                    auto iter = _file_manager.find(filename);
                    if(iter != _file_manager.end()){
                        delete (iter->second);
                    }
                }
                ToStorage();
                delete _worker_pool;
            }
            static int GetRowSize(){
                return CLFileManager::_s_row_size;
            }
            static bool InitInstance(int perFilesize,int rowSize,
                                    const std::string dirDes = "",
                                    const std::string& prefix = "_dms_file_",
                                    const std::string& suffix = ".txt"){
                CLFileManager::_s_row_size = rowSize;
                CLFileManager::_s_max_rows_for_per_file = perFilesize;
                CLFileManager::_s_dms_prefix = prefix;
                CLFileManager::_s_dms_suffix = suffix;
                return true;
            }
            static CLFileManager *GetInstance();
            void ToStorage(){
                _configure.SetConfigure(CLFileManager::_s_max_rows_for_per_file,
                        CLFileManager::_s_row_size);
                _configure.ToStorage();

                _meta_file.SetMetaData(_cur_total_rows);
                _meta_file.ToStorage();
            }
            bool FromStorage(){
                auto flag = _configure.FromStorage();
                if(flag){
                    CLFileManager::_s_row_size = _configure._config_row_size;
                    CLFileManager::_s_max_rows_for_per_file = _configure._config_max_rows_for_per_file;
                }
            }
            bool Append(int row,const char *buffer,int total){
                CLAbstractTableFile *pCLFile;

                if(!_FindCurCLFile(&pCLFile)){
                    _ExtendLocalCLFile();
                    _FindCurCLFile(&pCLFile);
                }
                if(pCLFile->WriteRow(buffer,total)){
                    _cur_total_rows+=1;
                } else{
                    return false;
                }
                if(_EqualMaxRow()){
                    pCLFile->Flush();
                }
                row = _cur_total_rows;
                return true;
            }
            bool Append(const std::string &msg);

            bool Read(int row,char *buf,int size);
            /*
             * multi thread api
             * */
            bool MultiRead(SLReaderMethodParam *param);

            bool MultiWrite(SLWriterMethodParam *param);
        };
        class CLMutilReaderFunc: public thread::CLExcutiveAbstractFunc{
        public:
            CLMutilReaderFunc(void *pContext):CLExcutiveAbstractFunc(pContext){}
            void RunFuncEntity(){
                SLReaderMethodParam *method = (SLReaderMethodParam*)(__runningContext);
                char buf[CLFileManager::GetRowSize()];
                CLFileManager::GetInstance()->Read(method->row,buf,CLFileManager::GetRowSize());
                method->callback(buf,CLFileManager::GetRowSize());
            }
            ~CLMutilReaderFunc() = default;
        };
        class CLMutilWriterFunc: public thread::CLExcutiveAbstractFunc{
        public:
            CLMutilWriterFunc(void *pContext):CLExcutiveAbstractFunc(pContext){}
            void RunFuncEntity(){
                SLWriterMethodParam *method = (SLWriterMethodParam*)(__runningContext);
                char buf[CLFileManager::GetRowSize()];
                int row;
                CLFileManager::GetInstance()->Append(row,buf,CLFileManager::GetRowSize());
                method->callback(row,buf,CLFileManager::GetRowSize());
            }
            ~CLMutilWriterFunc() = default;
        };
    }
}



#endif //LINUX_DMS_CLFILEMANAGER_H
