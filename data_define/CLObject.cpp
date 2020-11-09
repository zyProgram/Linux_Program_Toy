//
// Created by zhangyu on 2020/10/23.
//

#include "CLObject.h"
std::unordered_map<zy::dms::CLObjectType,size_t> zy::dms::CLUserAttrVector::_s_type_size_map =
        {
                {CLObjectType::Int,sizeof(int)},
                {CLObjectType::Char,sizeof(char)},
                {CLObjectType::Double,sizeof(double)}
        };
