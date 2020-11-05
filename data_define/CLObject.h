//
// Created by zhangyu on 2020/10/23.
//

#ifndef LINUX_DMS_CLOBJECT_H
#define LINUX_DMS_CLOBJECT_H

#include <vector>
#include <iostream>
#include <unordered_map>
#include "string"
#include "assert.h"
#include "functional"
namespace zy{
    namespace dms{
        class CLAbstractObject {
        public:
            virtual void ToCharBuffer(std::string &buf) = 0;
            virtual void ToCharBuffer(char *buf) = 0;
            virtual void FromCharBuffer(const char *buf) = 0;
            virtual void FromCharBuffer(const std::string &str) = 0;
            virtual int32_t GetBufferSize() = 0;
            virtual void Print()=0;
            virtual ~CLAbstractObject() = default;
        };
        //封装pod类型，常见于int short char float double float 自定义struct 类型的数据
        template <typename T,typename std::enable_if<std::is_pod<T>::value>* = nullptr>
        class CLDataObject: public CLAbstractObject{
        private:
            T _data;
        public:
            CLDataObject(T data){
                _data=data;
            }
            ~CLDataObject()=default;

            CLDataObject(const CLDataObject & o){
                _data = o.__data;
            }
            void ToCharBuffer(std::string &buf){
                memcpy((char *)(buf.c_str()), &_data, GetBufferSize());
            }
            void ToCharBuffer(char *buf){
                memcpy((char *)(buf), &_data, GetBufferSize());
            }
            void FromCharBuffer(const char *buf){
                memcpy(&_data, buf, GetBufferSize());
            }
            void FromCharBuffer(const std::string &buf){
                assert(buf.length()>=4);
                memcpy(&_data, buf.c_str(), GetBufferSize());
            }
            int32_t GetBufferSize(){
                return sizeof(T);
            }
            void Print(){
                std::cout<<"data:"<<_data<<" size:"<<GetBufferSize()<<std::endl;
            }
        };

        class CLObjectProduceFactory{
        public:
            template <typename T>
            CLAbstractObject *CreateObject(const T &attr){
                return new CLDataObject<T>(attr);
            }
        };

        /**
         * 存储单条信息
         * 每条信息包含 100个 CLObject
         * 统一用vector存储
         */
        class CLUserAttrVector: public CLAbstractObject{
        private:
            std::vector<CLAbstractObject *> _user_attr_vector;
        public:
            CLUserAttrVector(const CLUserAttrVector &) = delete;
            CLUserAttrVector &operator=(const CLUserAttrVector &) = delete;
            CLUserAttrVector(CLUserAttrVector &&o){
                for(auto &iter:o._user_attr_vector){
                    o.PushBack(iter);
                }
            }
            ~CLUserAttrVector(){
                Clear();
            }
            void Clear(){
                for(auto &iter:_user_attr_vector){
                    if(iter!= nullptr){
                        delete iter;
                        iter = nullptr;
                    }
                }
            }
            void PushBack(CLAbstractObject *p){
                _user_attr_vector.push_back(p);
            }
            CLUserAttrVector() = default;
            void ToCharBuffer(std::string &buf){
                if(buf.capacity() < GetBufferSize()){
                    buf.reserve(GetBufferSize() - buf.capacity());
                }
                int offset = 0;
                for(auto &iter:_user_attr_vector){
                    int bufsize =iter->GetBufferSize();
                    char tempbuf[bufsize];
                    iter->ToCharBuffer(tempbuf);
                    memcpy((char*)(buf.data()+offset),tempbuf,bufsize);
                    offset+=bufsize;
                }
            }
            void ToCharBuffer(char *buf){
                int offset = 0;
                for(auto &iter:_user_attr_vector){
                    iter->ToCharBuffer(buf+offset);
                    offset+=iter->GetBufferSize();
                }
            }
            void FromCharBuffer(const char *buf){
                int offset = 0;
                CLAbstractObject *p = nullptr;
                for(auto &iter:_user_attr_vector){
                    iter->FromCharBuffer(buf+offset);
                    offset+=iter->GetBufferSize();
                }
            }
            void FromCharBuffer(const std::string &buf){
                assert(buf.capacity() >= GetBufferSize());
                int offset = 0;
                for(auto &iter:_user_attr_vector){
                    iter->FromCharBuffer(buf.data()+offset);
                    offset+=iter->GetBufferSize();
                }
            }
            int32_t GetBufferSize(){
                int total = 0;
                for(auto &iter:_user_attr_vector){
                    total+=iter->GetBufferSize();
                }
                return total;
            }
            void Print(){
                std::cout<<"user attr vector info:"<<" size:"<<GetBufferSize()<<std::endl;
                for(auto &iter:_user_attr_vector){
                    if(iter!= nullptr){
                         iter->Print();
                    }
                }
            }
        };
    }
}



#endif //LINUX_DMS_CLOBJECT_H
