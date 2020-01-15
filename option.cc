#include "option.h"

#include <unistd.h>
#include <getopt.h>

#include <iostream>
#include <string>

namespace voip{


	void Useage()
	{
		std::cout<<"Useage : ./name.out  -e encryption_method -g encoder_method -f wav_file_name "<<std::endl;

	}

	//获取程序开始的参数选项
	const Options init(int argc,char **argv)
	{
			//加密方式和编码方式
			int encryption_method,encoder_method;
			//处理的wav 文件名
			std::string wav_file_name;
			int opt;
			int count_arg = 0;
			opterr = 0;

			while( (opt = getopt(argc,argv,"e:g:f:")) != -1){
				switch(opt){
					case 'e':
						encryption_method = atoi(optarg);
						++count_arg;
						break;

					case 'g':
						encoder_method = atoi(optarg);
						++count_arg;
						break;


					case 'f':
						wav_file_name = optarg;
						++count_arg;
						break;

					default:
						Useage();	//如果是？的情况
						exit(1);

				}
			}

			if(count_arg != 3){
				Useage();	//如果是？的情况
				exit(1);
			}
			else
				return Options(encryption_method,encoder_method,wav_file_name);

	}

}