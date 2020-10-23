cmake_minimum_required(VERSION 3.17)
project(Linux_DMS)

set(CMAKE_CXX_STANDARD 11)
set(ThreadFile utility/thread/CLLock.cpp utility/thread/CLLock.h
        utility/thread/CLExcutive.cpp utility/thread/CLExcutive.h
        utility/thread/CLConditionVar.cpp utility/thread/CLConditionVar.h
        utility/thread/CLLockGuard.cpp utility/thread/CLLockGuard.h
        utility/thread/CLEvent.cpp utility/thread/CLEvent.h)

set(IOFile utility/file/CLFile.cpp utility/file/CLFile.h
        utility/file/CLBuffer.cpp utility/file/CLBuffer.h
        utility/file/CLLogger.cpp utility/file/CLLogger.h )

set(AppFile utility/app/CLTableFile.cpp utility/app/CLTableFile.h
        utility/app/CLFileManager.cpp utility/app/CLFileManager.h
        utility/app/CLTableConfigure.cpp utility/app/CLTableConfigure.h)

set(NetFile utility/network/CLNetwork.cpp utility/network/CLNetwork.h utility/thread/CLAtomicLock.cpp utility/thread/CLAtomicLock.h)


include_directories(utility)
add_executable(Linux_DMS main.cpp ${IOFile}
        ${ThreadFile}
        ${AppFile}
        ${NetFile})