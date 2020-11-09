//
// Created by zhangyu on 2020/10/22.
//

#ifndef LINUX_DMS_CLTABLECONFIGURE_H
#define LINUX_DMS_CLTABLECONFIGURE_H
#include "CLFileManager.h"
namespace zy{
    namespace dms{
        class CLTableConfigure {
        private:
            std::string _s_row_size;
            std::string _s_max_rows_for_per_file;
            std::string _max_file_index;
            std::string _cur_total_rows;
        public:
            void ToStorage(){
                zy::file::CLFile file(CLFileManager::_s_meta_data_file_prefix+
                                      CLFileManager::_s_dms_prefix);
                file.Clear();
                std::unordered_map<std::string,std::string> data;
                data.emplace("_s_row_size",std::to_string(CLFileManager::_s_row_size));
                data.emplace("_s_max_rows_for_per_file",std::to_string(CLFileManager::_s_max_rows_for_per_file));
                data.emplace("_max_file_index",std::to_string(_max_file_index));
                data.emplace("_cur_total_rows",std::to_string(_cur_total_rows));
                for(auto &iter:data){
                    std::string str(iter.first+':'+iter.second+'\n');
                    file.Write(str.c_str(),str.length());
                }

            }
            bool FromStorage(){
                zy::file::CLFile file(CLFileManager::_s_meta_data_file_prefix+
                                      CLFileManager::_s_dms_prefix);
                std::unordered_map<std::string,std::string> data;
                int total = 4;
                data.emplace("_s_row_size",std::to_string(CLFileManager::_s_row_size));
                data.emplace("_s_max_rows_for_per_file",std::to_string(CLFileManager::_s_max_rows_for_per_file));
                data.emplace("_max_file_index",std::to_string(_max_file_index));
                data.emplace("_cur_total_rows",std::to_string(_cur_total_rows));
                int offset = 0;
                for(auto &iter:data){

                }
            }
        };
    }
}



#endif //LINUX_DMS_CLTABLECONFIGURE_H
