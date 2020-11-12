//
// Created by zhangyu on 2020/10/22.
//

#ifndef LINUX_DMS_CLTABLECONFIGURE_H
#define LINUX_DMS_CLTABLECONFIGURE_H

#include <sstream>
#include "CLTableConfigure.h"
#include "../utility/file/CLFile.h"
#include "unordered_map"


#define MAX_BUF_FILE_SIZE 256
namespace zy{
    namespace dms{
        class CLTableConfigure {
        private:
            std::string _file_name;
            bool _restart = false;
            void _Dump(int row_size,int max_rows_for_per_file,
                        int max_file_index,int cur_total_rows){
                zy::file::CLFile file(_file_name);
                file.Clear();
                int total = 0;
                std::unordered_map<std::string,std::string> data;
                data.emplace("_s_row_size",std::to_string(row_size));
                data.emplace("_s_max_rows_for_per_file",std::to_string(max_rows_for_per_file));
                data.emplace("_max_file_index",std::to_string(max_file_index));
                data.emplace("_cur_total_rows",std::to_string(cur_total_rows));
                std::stringstream ss;
                for(auto &iter:data){
                    ss<<iter.first+':'+iter.second+'\n';
                }
                total = ss.str().length();
                file.Write((char *)&total, sizeof(int));
                file.Write(ss.str().c_str(),total);
                file.Close();
            }
        public:
            int _s_row_size;
            int _s_max_rows_for_per_file;
            int _max_file_index;
            long long _cur_total_rows;
            CLTableConfigure(const std::string &filename = "meta_data.txt"){
                _file_name = filename;
            }
            bool IsRestart(){
                return _restart;
            }
            void ToStorage(){
                _Dump(_s_row_size,_s_max_rows_for_per_file,
                      _max_file_index,_cur_total_rows);
            }
            bool FromStorage(){
                try{
                    zy::file::CLFile file(_file_name);
                    if(file.IsEmpty()){
                        return true;
                    }
                    char buf[MAX_BUF_FILE_SIZE];
                    int real = 0;
                    file.ReadAll(buf,MAX_BUF_FILE_SIZE,real);
                    if(real == -1){
                        return false;
                    }
                    bool isRecordingKey = true;
                    std::string key,value;
                    std::unordered_map<std::string,std::string> data;
                    for(int i=0;i<real;i++){
                        auto iter = buf[i];
                        if(iter == '\n'){
                            isRecordingKey = true;
                            data.emplace(key,value);
                            key.clear();
                            value.clear();
                        }else if(iter == ':'){
                            isRecordingKey = false;
                        }else{
                            isRecordingKey?(key.push_back(iter)):(value.push_back(iter));
                        }
                    }
                    try {
                        _s_row_size = std::stoi(data.at("_s_row_size"));
                        _s_max_rows_for_per_file = std::stoi(data.at("_s_max_rows_for_per_file"));
                        _max_file_index = std::stoi(data.at("_max_file_index"));
                        _cur_total_rows = std::stoll(data.at("_cur_total_rows"));
                    }catch (std::out_of_range &e){
                        throw std::string("configure file of "+_file_name+" has error:"+e.what());
                    }
                }catch (std::string &e){
                    std::cout<<__FUNCTION__<<":"<<e<<std::endl;
                    return false;
                }
                _restart = true;
                return true;
            }
        };
    }
}



#endif //LINUX_DMS_CLTABLECONFIGURE_H
