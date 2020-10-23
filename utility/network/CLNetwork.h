//
// Created by zhangyu on 2020/10/22.
//

#ifndef LINUX_DMS_CLNETWORK_H
#define LINUX_DMS_CLNETWORK_H

#include <iostream>
#include "string"
#include "windows.h"
#include "unistd.h"
#include "string.h"
#include "errno.h"

namespace zy{
    namespace network{
        class CLAbstractNetwork {
        public:
            virtual bool Read(char *buffer,int size) = 0;
            virtual bool Write(const char *buffer, int size) = 0;
            virtual bool Close() = 0;
            virtual ~CLAbstractNetwork() = 0;
        };

        class CLTCPNetwork: public CLAbstractNetwork{
        private:
            int _port;
            std::string _ip_address;
            bool _connected = false;
            int _fd;
            bool _Connect(){
                if(!_connected){
                    sockaddr_in servaddr;
                    servaddr.sin_family = AF_INET;
                    servaddr.sin_port = htons(_port);
                    servaddr.sin_addr.s_addr = inet_addr(_ip_address.c_str());
                    return ::connect(_fd,(sockaddr*)(&servaddr),sizeof(servaddr))>0;
                }
                return true;
            }
        public:
            CLTCPNetwork(std::string &ipAddress,int port){
                _ip_address = ipAddress;
                _port = port;
                if(_fd = ::socket(AF_INET,SOCK_STREAM,IPPROTO_IP) < 0){
                    throw std::string("socket < 0 error:")+strerror(errno);
                }
            }
            bool Write(const char *buffer, int size){
                if(!_connected && !_Connect()){
                    std::cout<<"tcp error connect fail:"<<strerror(errno)<<std::endl;
                    return false;
                }
                int curSend = 0;
                int totalSend = 0;
                int headSize = sizeof(int32_t);
                while(totalSend != headSize) {
                    if ((curSend = ::write(_fd, ((char*)(&headSize))+totalSend, headSize - totalSend)) < 0) {
                        std::cout<<"send error"<<std::endl;
                        return false;
                    }
                    totalSend+=curSend;
                }
                curSend = 0; //发送计数清空
                totalSend = 0;
                while(totalSend != size) {
                    if ((curSend = ::write(_fd, buffer + totalSend, size - totalSend)) < 0) {
                        std::cout<<"send error"<<std::endl;
                        return false;
                    }
                    totalSend+=curSend;
                }
                return true;
            }
            bool start(){

            }
            bool Read(char *buffer, int size){
                if(!_connected && !_Connect()){
                    std::cout<<"tcp error connect fail:"<<strerror(errno)<<std::endl;
                    return false;
                }
                int curSend = 0;
                int totalSend = 0;
                while(totalSend != size) {
                    if ((curSend = ::read(_fd, buffer + totalSend, size - totalSend)) < 0) {
                        std::cout<<"send error"<<std::endl;
                        return false;
                    }
                    totalSend+=curSend;
                }
                return true;
            }
        };
    }
}



#endif //LINUX_DMS_CLNETWORK_H
