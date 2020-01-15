#include "consumer.h"

#include <cstring>
#include <cerrno>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/time.h>
#include <unistd.h>


namespace voip {

	//��ȡ��ǰ��ʱ��
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


	//PCAP�Ĺ��캯��
	PcapConsumer::PcapConsumer(std::string file_name)
	{
		//�ļ���С
		m_file_size = 0;

		//���û���ṩ�ļ�������ô���Լ�����һ���ļ���
		if ("" == file_name)
			GenerateFileName();
		//����һ����
		m_file_stream.open(m_file_name.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);
		//�����ʧ��
		if (!(m_file_stream.is_open()))
		{
			std::cerr << __FILE__ << " " << __LINE__ << " output stream is not able to be added. Filename : " << m_file_name << std::endl;
			return;
		}
		//�ļ�ͷ�Ĵ�С
		int pcap_file_hdr_size = sizeof(m_pcap_file_header);
		//д���ļ�ͷ��С
		m_file_stream.write((char*)(&m_pcap_file_header), pcap_file_hdr_size);
		//�����ļ���С��ֻ��һ���ļ�ͷ
		m_file_size = pcap_file_hdr_size;
	}

	PcapConsumer::~PcapConsumer()
	{
		//��������ļ�������ô�ر�
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
		//�����ļ���
		m_file_name = std::string(time_part) + ".pcap";
	}

	//���Ѵ���
	bool PcapConsumer::Consume(const unsigned char* data_ptr, unsigned short int data_size)
	{
		// ����һ��pacp�����ݰ��İ�ͷ
		PcapPacHdrType pcap_packet_header;

		// ��ȡ��ǰ��ʱ��
		GetCurrentTimeInTv(pcap_packet_header.ts_sec, pcap_packet_header.ts_usec);
		//��ͷ�������ݴ�С����Ϣ
		pcap_packet_header.incl_len = data_size;
		pcap_packet_header.orig_len = data_size;

		//��ȡ��ͷ�Ĵ�С
		int pcap_packet_header_size = sizeof(PcapPacHdrType);

		//����ļ���û�д�
		if (!(m_file_stream.is_open()))
		{
			//����
			m_file_stream.open(m_file_name.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::app);

			if (!(m_file_stream.is_open()))
			{
				std::cerr << __FILE__ << " " << __LINE__ << " output stream is not able to be added. Filename : " << m_file_name << std::endl;
				return false;
			}
		}

		m_file_stream.write((char*)(&pcap_packet_header), pcap_packet_header_size);
		//�ļ���С ���ϰ�ͷ�Ĵ�С
		m_file_size += pcap_packet_header_size;

		m_file_stream.write((const char*)data_ptr, data_size);
		//�ļ���С �������ݶεĴ�С
		m_file_size += data_size;

		m_file_stream.flush();

		return true;
	}

}