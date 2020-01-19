//
// Created by ape on 2020/1/12.
//

#ifndef FILE_MYFILE_H
#define FILE_MYFILE_H

#include <iostream>
#include <string>
#include <stdio.h>

namespace  voip{
    //输出文件
   class OutFile{
   private:
      std::string  m_fileName;
      FILE  *m_fp;
   public:
       explicit  OutFile(std::string fileName):
           m_fileName(fileName)
       {
           m_fp = fopen(m_fileName.c_str(),"ab+");
           if(!m_fp)
               std::cerr<<m_fileName<<" open fail"<<std::endl;
           else
               std::cout<<m_fileName<<" open success"<<std::endl;
       }

       bool WriteToFile(const char* dataPtr,size_t dataLen);

       ~OutFile()
       {
           if(m_fp){
               fflush(m_fp);
               fclose(m_fp);
           }
       }
   };

   //输入文件
   class InFile{
   private:
       std::string m_fileName;
       FILE *m_fp;
   public:
       explicit  InFile(std::string fileName):
               m_fileName(fileName)
       {
           m_fp = fopen(m_fileName.c_str(),"rb");
           if(!m_fp)
               std::cerr<<m_fileName<<" open fail"<<std::endl;
       }


       bool ReadFromFile(char* dataPtr, size_t dataLen);

       //获取文件大小,返回字节数
       size_t GetFileSize() const ;
       
       ~InFile()
       {
           if(m_fp)
               fclose(m_fp);
       }

   };
}
#endif //FILE_MYFILE_H
