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

namespace zy{
    namespace dms{

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
            /**
             * 嵌套类实现 内存回收，通过静态的嵌套类 析构来 delete instance，也可以直接用atexist()
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

            bool _FindCLFile(int row,CLAbstractTableFile **pFile){
                if(row > _cur_total_rows){
                    std::cout << "exist current max rows:" << row << " > " << _cur_total_rows << std::endl;
                    return false;
                }
                int index = row / (CLFileManager::_s_max_rows_for_per_file+1) + 1;
                std::string filename = _ConstructFileName(index);
                *pFile = _file_manager.at(filename);
                return true;
            }
            bool _EqualMaxRow(){
                return 0 ==  _cur_total_rows % _s_max_rows_for_per_file;
            }
            bool _FindCurCLFile(CLAbstractTableFile **pFile){
                int newRow = _cur_total_rows + 1;
                int index = newRow / (CLFileManager::_s_max_rows_for_per_file+1) + 1;
                if(index > _max_file_index){
                    return false;
                }
                std::string filename = _ConstructFileName(index);
                *pFile = _file_manager.at(filename);
                return true;
            }
            std::string _ConstructFileName(int index){
                return _s_dms_prefix+std::to_string(index)+_s_dms_suffix;
            }
            bool _ExtendLocalCLFile(){
                int orginIndex = _max_file_index;
                _max_file_index += 10;
                try {
                    std::string filename;
                    for(int i=orginIndex+1;i<=_max_file_index;i++){
                        filename = _ConstructFileName(i);
                        _file_manager.emplace(filename,new CLTableFile(filename,CLFileManager::_s_row_size));
                    }
                } catch (std::string &errMsg) {
                    std::cout<<"write new data. must extend file,but "<<errMsg<<std::endl;
                    return false;
                }
                return true;
            }
        public:
            CLFileManager(){
                //TODO 处理文件夹
                _cur_total_rows = 0;
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
            static CLFileManager *GetInstance(){
                if(_s_instance != nullptr){
                    return _s_instance;
                }
                _s_create_instance_lock.Lock();
                try {
                    if (_s_instance == nullptr) {
                        if (CLFileManager::_s_row_size == 0) {
                            throw std::string("init first");
                        }
                        _s_instance = new CLFileManager();
                    }
                } catch (std::string &errMsg) {
                    _s_create_instance_lock.UnLock();
                    throw errMsg;
                }
                _s_create_instance_lock.UnLock();
                return _s_instance;
            }
            void ToStorage(){
                zy::dms::CLTableConfigure configure;
                configure._cur_total_rows = _cur_total_rows;
                configure._max_file_index = _max_file_index;
                configure._s_max_rows_for_per_file = CLFileManager::_s_max_rows_for_per_file;
                configure._s_row_size = CLFileManager::_s_row_size;
                configure.ToStorage();
            }
            bool FromStorage(){
                zy::dms::CLTableConfigure configure;
                auto flag = configure.FromStorage();
                if(flag){
                    CLTableConfigure::_s_row_size = configure._s_row_size;
                    CLTableConfigure::_s_max_rows_for_per_file = configure._s_max_rows_for_per_file;
                }
            }
            bool Append(const char *buffer,int total){
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
                return true;
            }
            bool Append(const std::string &msg){
                CLAbstractTableFile *pCLFile;
                if(!_FindCurCLFile(&pCLFile)){
                    _ExtendLocalCLFile();
                    _FindCurCLFile(&pCLFile);
                }

                if(pCLFile->WriteRow(msg)){
                    _cur_total_rows+=1;
                } else{
                    return false;
                }
                if(_EqualMaxRow()){
                    pCLFile->Flush();
                }
                return true;
            }

            bool Read(int row,char *buf,int size){
                CLAbstractTableFile *pCLFile;
                if(!_FindCLFile(row,&pCLFile)){
                    return false;
                }
                int calRow = row % (CLFileManager::_s_max_rows_for_per_file);
                calRow = ((calRow == 0) ? CLFileManager::_s_max_rows_for_per_file:calRow); //最后一行

                return pCLFile->ReadRow(calRow,buf,size);
            }
        };

    }
}



#endif //LINUX_DMS_CLFILEMANAGER_H
