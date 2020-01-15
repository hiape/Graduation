#include <iostream>
#include <string>

#include "option.h"
#include "consumer.h"


using namespace voip;
using namespace std;


int main(int argc,char **argv)
{

	Options sys_option = init(argc,argv);
	
	std::cout<<sys_option.GetWavFileName()<<std::endl;
	std::cout<<sys_option.GetEncryptionOption()<<std::endl;
	std::cout<<sys_option.GetEncoderOption()<<std::endl;
	
	//创建一个消费者
	Consumer* consumer_ptr;
	consumer_ptr = new PcapConsumer();

	EncoderFactory* encoder_factory;

	//编码器的选择
	switch(sys_option.GetEncoderOption()){
		case kG711a:
			encoder_factory = new G711aEncoderFactory();
			break;
		case kG711u:
			encoder_factory = new G711uEncoderFactory();
			break;
		default :
			std::cout<<"none of this Encoder method"<<std::endl;
			exit(1);
			break;
	}

	//加密方式的选择
	switch(sys_option.GetEncryptionOption()){
		
	}


	//资源的释放
	if(consumer_ptr)
		delete consumer_ptr;
	if(encoder_factory)
		delete encoder_factory;
	return 0;
}