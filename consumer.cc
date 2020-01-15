#include "consumer.h"

#include <cstring>
#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>


namespace voip {

	//获取当前的时间
	void GetCurrentTimeInTv(unsigned int& sec, unsigned int& usec)
	{
		timeval tv;
		if (0 == gettimeofday(&tv, NULL))
		{
			sec = (unsigned int)tv.tv_sec;
			usec = (unsigned int)tv.tv_usec;
		}
		else
			std::cerr << __FILE__ << " " << __LINE__ << " unable to obtain time info" << std::endl;
	}


	//PCAP的构造函数
	PcapConsumer::PcapConsumer(std::string file_name)
	{
		//文件大小
		m_file_size = 0;

		//如果没有提供文件名，那么会自己生成一个文件名
		if ("" == file_name)
			GenerateFileName();
		//创建一个流
		m_file_stream.open(m_file_name.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);
		//如果打开失败
		if (!(m_file_stream.is_open()))
		{
			std::cerr << __FILE__ << " " << __LINE__ << " output stream is not able to be added. Filename : " << m_file_name << std::endl;
			return;
		}
		//文件头的大小
		int pcap_file_hdr_size = sizeof(m_pcap_file_header);
		//写入文件头大小
		m_file_stream.write((char*)(&m_pcap_file_header), pcap_file_hdr_size);
		//现在文件大小就只有一个文件头
		m_file_size = pcap_file_hdr_size;
	}

	PcapConsumer::~PcapConsumer()
	{
		//如果打开了文件流，那么关闭
		if (m_file_stream.is_open())
			m_file_stream.close();

		m_file_name.clear();
		m_file_stream.clear();
		m_file_size = 0;
	}

	void PcapConsumer::GenerateFileName()
	{
		time_t call_start_sec_tt;
		time(&call_start_sec_tt);

		tm* call_time_ptr = localtime(&call_start_sec_tt);
		tm call_time = *call_time_ptr;

		const int time_part_size = 20; //4+1+2+1+2+1+2+1+2+1+2+1
		char time_part[time_part_size];

		int snprintf_result = snprintf(time_part, time_part_size, "%d %.2d %.2d %.2d %.2d %.2d", ((call_time.tm_year) + 1900),
			((call_time.tm_mon) + 1), (call_time.tm_mday), (call_time.tm_hour), (call_time.tm_min),
			(call_time.tm_sec));

		if ((snprintf_result <= 0) || (snprintf_result >= time_part_size))
		{
			std::cerr << __FILE__ << " " << __LINE__ << "unable to execute snprintf" << std::endl;
			m_file_name = "TestFile.pcap";
			return;
		}
		//设置文件名
		m_file_name = std::string(time_part) + ".pcap";
	}

	//消费代码
	bool PcapConsumer::Consume(const unsigned char* data_ptr, unsigned short int data_size)
	{
		// 创建一个pacp的数据包的包头
		PcapPacHdrType pcap_packet_header;

		// 获取当前的时间
		GetCurrentTimeInTv(pcap_packet_header.ts_sec, pcap_packet_header.ts_usec);
		//包头填入数据大小的信息
		pcap_packet_header.incl_len = data_size;
		pcap_packet_header.orig_len = data_size;

		//获取包头的大小
		int pcap_packet_header_size = sizeof(PcapPacHdrType);

		//如果文件流没有打开
		if (!(m_file_stream.is_open()))
		{
			//打开流
			m_file_stream.open(m_file_name.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);

			if (!(m_file_stream.is_open()))
			{
				std::cerr << __FILE__ << " " << __LINE__ << " output stream is not able to be added. Filename : " << m_file_name << std::endl;
				return false;
			}
		}

		m_file_stream.write((char*)(&pcap_packet_header), pcap_packet_header_size);
		//文件大小 加上包头的大小
		m_file_size += pcap_packet_header_size;

		m_file_stream.write((const char*)data_ptr, data_size);
		//文件大小 加上数据段的大小
		m_file_size += data_size;

		m_file_stream.flush();

		return true;
	}

}