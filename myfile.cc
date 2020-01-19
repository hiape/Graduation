//
// Created by ape on 2020/1/12.
//

#include "myfile.h"

#include <stdio.h>
#include <sys/stat.h> 
#include <unistd.h>

namespace  voip{

    bool  OutFile::WriteToFile(const char *dataPtr, size_t dataLen) {
        do{
            if(!dataPtr){
                break;
            }
            if(!m_fp){
                break;
            }
            size_t len = fwrite(dataPtr,dataLen,1,m_fp);
            if( len == dataLen)
                return true;
            else
                break;
        }while(0);
        return false;
    }


    bool InFile::ReadFromFile(char *dataPtr, size_t dataLen) {
        do{
            if(!dataPtr){
                break;
            }
            if(!m_fp){
                break;
            }
            size_t len = fread(dataPtr,dataLen,1,m_fp);
            if(len == dataLen)
                return true;
            else
                break;
        }while(0);
        return false;
    }

    size_t  InFile::GetFileSize() const {
        //如果文件打开失败
        if(!m_fp)
            return -1;
        struct stat file_info;
        int ret = stat(m_fileName.c_str(),&file_info);
        //如果stat函数返回出错
        if( ret == -1)
            return -1;
        return file_info.st_size;
    }


}
