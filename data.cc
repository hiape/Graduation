#include "data.h"
#include "encoder.h"
#include "encryption.h"
#include "myfile.h"
#include "fileformat.h"

namespace voip{


  	PcmData::PcmData(Encoder* encoder_ptr, Encryption* encryption_ptr,InFile& wav_file):
                m_encoder(encoder_ptr),
                m_encryption(encryption_ptr),
                m_wavfile(wav_file),
                m_dataSize(0),
                m_dataPtr(nullptr)
    {
    	//判断指针对象不为空
    	assert(m_encoder && m_encryption);

        //先读取WAV文件头，获取PCM裸流的属性
        

        //获取编码器的属性

        //查看PCM裸流的属性

        //是否需要重采样的操作
       


    }


    //资源释放
    PcmData::~PcmData()
    {

    	if(m_encoder)
    		delete m_encoder;
    	if(m_encryption)
    		delete m_encryption;

    }


}