#ifndef _OPTION_H
#define _OPTION_H

#include <string>

namespace voip{



	void Useage() ;

	enum EncryptionOpton
	{
		kNoneEncryption,
		kAES,
		kAESb,
	};

	enum EncoderOption
	{
		kG711a,
		kG711u,
		kG722,
		kG723,
	};


	class Options{
	private:
		enum EncryptionOpton  m_encryption;   // 加密方法
		enum EncoderOption    m_encoder;      //编码方式
		std::string           m_wavFileName;  //需要处理的wav文件名称
	public:
		Options(int encryption,int encoder,std::string fileName):
				m_encryption(static_cast<enum EncryptionOpton>(encryption)),
				m_encoder(static_cast<enum EncoderOption>(encoder)),
				m_wavFileName(fileName)
		{}

		Options() = default;

		~Options() {}

		const std::string& GetWavFileName() const { return m_wavFileName;}
		int GetEncryptionOption () const {return m_encryption;}
		int GetEncoderOption() const { return m_encoder;}
	};


const Options init(int argc,char **argv);
}

#endif