cmake_minimum_required(VERSION 3.17)
project(Linux_DMS)

set(CMAKE_CXX_STANDARD 11)
aux_source_directory(utility/thread ThreadFile)

aux_source_directory(utility/file IOFile)

aux_source_directory(utility/app AppFile)

set(NetFile utility/network/CLNetwork.cpp utility/network/CLNetwork.h
        utility/thread/CLAtomicLock.cpp utility/thread/CLAtomicLock.h)

set(ObjectFile data_define/CLObject.h data_define/CLObject.cpp)
include_directories(utility)
add_executable(Linux_DMS main.cpp ${IOFile}
        ${ThreadFile}
        ${AppFile}
        ${NetFile})
target_link_libraries(Linux_DMS pthread)

add_executable(DMS_Object_Test data_define/TestUserData.cpp
              ${ObjectFile}
              ${ThreadFile}
              ${AppFile}
              ${IOFile})
target_link_libraries(DMS_Object_Test pthread)