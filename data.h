/*************************************************************************
	> File Name: data.h
	> Author:ape 
	> Mail:zw1993@163.com 
	> Created Time: 2020年01月14日 星期二 10时23分22秒
 ************************************************************************/

#ifndef _DATA_H
#define _DATA_H

#include "assert.h"
#include <string>

#include "myfile.h"
#include "fileformat.h"

namespace voip{

    class Encoder;
    class Encryption;
    class InFile;

    //PCM裸流
    class PcmData{
    private:
        Encoder*   m_encoder;     //编码方式
        Encryption* m_encryption;  //加密方式
        InFile&     m_wavfile;    //wav文件

        WavHeader   m_wavheader;   //pcm裸流对应的wav文件头

        size_t m_dataSize;      //数据的大小
        char* m_dataPtr;        //数据地址
        std::string m_lable;     //标签


    public:
        PcmData(Encoder* encoder_ptr, Encryption* encryption_ptr,InFile& wav_file);
              
     
        ~PcmData() ;
    
        virtual char* GetDataPtr() const { return m_dataPtr;}
        virtual size_t GetDataSize() const { return m_dataSize;}
        //获取数据标签
        virtual std::string GetDataLabel() const { return m_lable;}

        //获取该段内容的数据标签,语音的内容是什么
        std::string GetContentLabel() const { return string("content");}


    private:
        //从WAV文件中读取所有的PCM裸流
        bool ReadPcmFromFile();

    };

   

}


#endif
