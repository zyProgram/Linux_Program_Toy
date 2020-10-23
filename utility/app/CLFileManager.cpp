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