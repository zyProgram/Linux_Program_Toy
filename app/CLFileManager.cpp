//
// Created by zhangyu on 2020/10/22.
//

#include "CLFileManager.h"
int zy::dms::CLFileManager::_s_max_rows_for_per_file;
std::string zy::dms::CLFileManager::_s_dms_prefix;
std::string zy::dms::CLFileManager::_s_dms_suffix;
zy::thread::CLLock zy::dms::CLFileManager::_s_create_instance_lock;
zy::dms::CLFileManager* zy::dms::CLFileManager::_s_instance = nullptr;
int zy::dms::CLFileManager::_s_row_size = 0;
std::string zy::dms::CLFileManager::_s_meta_data_file_prefix = "_meta_data_of_";
zy::dms::CLFileManager::CLManagerGC zy::dms::CLFileManager::_s_gc;
int zy::dms::CLFileManager::_s_auto_increace_file_num = 4;
int zy::dms::CLFileManager::_s_thread_num;
namespace zy{
    namespace dms {
        bool CLFileManager::_ExtendLocalCLFile() {
            int orginIndex = _max_file_index;
            _max_file_index += _s_auto_increace_file_num;
            try {
                std::string filename;
                for (int i = orginIndex + 1; i <= _max_file_index; i++) {
                    filename = _ConstructFileName(i);
                    _file_manager.emplace(filename, new CLTableFile(filename, CLFileManager::_s_row_size));
                }
            } catch (std::string &errMsg) {
                std::cout << "write new data. must extend file,but " << errMsg << std::endl;
                return false;
            }
            return true;
        }

        bool CLFileManager::_FindCLFile(int row, CLAbstractTableFile **pFile) {
            if (row > _cur_total_rows) {
                std::cout << "exist current max rows:" << row << " > " << _cur_total_rows << std::endl;
                return false;
            }
            int index = row / (CLFileManager::_s_max_rows_for_per_file + 1) + 1;
            std::string filename = _ConstructFileName(index);
            try {
                *pFile = _file_manager.at(filename);
            }catch(std::out_of_range &e) {
                std::cout<<"dms catch "<<e.what()<<" when read "<<filename<<",so create file for reader"<<std::endl;
                *pFile = new CLTableFile(filename,CLFileManager::_s_row_size);
                _file_manager.emplace(filename,*pFile);
            }
            return true;
        }


        CLFileManager *CLFileManager::GetInstance() {
            if (_s_instance != nullptr) {
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

        bool CLFileManager::Append(const std::string &msg) {
            CLAbstractTableFile *pCLFile;
            if (!_FindCurCLFile(&pCLFile)) {
                _ExtendLocalCLFile();
                _FindCurCLFile(&pCLFile);
            }

            if (pCLFile->WriteRow(msg)) {
                IncreaseRow();
            } else {
                return false;
            }
            if (_EqualMaxRow()) {
                pCLFile->Flush();
            }
            return true;
        }

        bool CLFileManager::Read(int row, char *buf, int size) {
            CLAbstractTableFile *pCLFile;
            if (!_FindCLFile(row, &pCLFile)) {
                return false;
            }
            int calRow = row % (CLFileManager::_s_max_rows_for_per_file);
            calRow = ((calRow == 0) ? CLFileManager::_s_max_rows_for_per_file : calRow); //最后一行

            return pCLFile->ReadRow(calRow, buf, size);
        }

        bool CLFileManager::_FindCurCLFile(CLAbstractTableFile **pFile) {
            int newRow = _cur_total_rows + 1;
            int index = newRow / (CLFileManager::_s_max_rows_for_per_file + 1) + 1;
            if (index > _max_file_index) {
                return false;
            }
            std::string filename = _ConstructFileName(index);
            *pFile = _file_manager.at(filename);
            return true;
        }

        bool CLFileManager::MultiRead(SLReaderMethodParam *param) {
            auto *func = new CLMutilReaderFunc(param);
            return _worker_pool->Submit(func);
        }

        bool CLFileManager::MultiWrite(SLWriterMethodParam *param) {
            auto *func = new CLMutilWriterFunc(param);
            return _worker_pool->Submit(func);
        }
    }
}
