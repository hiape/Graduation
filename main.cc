#include <iostream>
#include <string>

#include "option.h"
#include "consumer.h"
#include "encoder.h"
#include "encryption.h"
#include "myfile.h"

using namespace voip;
using namespace std;


int main(int argc,char **argv)
{

	//获取系统的参数
	Options sys_option = init(argc,argv);
	
	//测试系统的参数输入
	std::cout<<sys_option.GetWavFileName()<<std::endl;
	std::cout<<sys_option.GetEncryptionOption()<<std::endl;
	std::cout<<sys_option.GetEncoderOption()<<std::endl;
	
	//创建一个消费者
	Consumer* consumer_ptr;
	consumer_ptr = new PcapConsumer();

	//创建一个编码器工厂， 由编码器工厂返回编码器对象
	EncoderFactory* encoder_factory_ptr;

	//编码器的选择
	switch(sys_option.GetEncoderOption()){

		case kG711a:
			encoder_factory_ptr = new G711aEncoderFactory();
			break;

		case kG711u:
			encoder_factory_ptr = new G711uEncoderFactory();
			break;

		default :
			std::cerr<<"none of this Encoder method"<<std::endl;
			exit(1);
			break;
	}

	//获取编码器的标签
	string data_label = encoder_factory_ptr->GetEncoderName();

	//创建一个加密工厂
	EncryptionFactory* encryption_factory_ptr = new AesEncryptionFactory();
	//加密方式的选择
	switch(sys_option.GetEncryptionOption()){

		case kNoneEncryption:
			encryption_factory_ptr = new NoEncryptionFactory();
			break;

		case kAES:
			encryption_factory_ptr = new AesEncryptionFactory();
			break;

		default:
			std::cerr<<"none of this Encryption Method"<<std::endl;
			exit(1);
			break;
	}

	//标签的叠加，加上加密方法的标签
	data_label = data_label + encryption_factory_ptr->GetEncryptionName();


	//打开需要处理的文件
	InFile wav_file(sys_option.GetWavFileName());

	//重采样的处理

	



	//资源的释放
	if(consumer_ptr)
		delete consumer_ptr;
	if(encoder_factory_ptr)
		delete encoder_factory_ptr;
	if(encryption_factory_ptr)
		delete encryption_factory_ptr;

	return 0;
}