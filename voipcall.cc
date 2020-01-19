#include "voipcall.h"
#include "generator.h"

#include <netinet/in.h>
#include <iostream>
#include <chrono>
#include <limits.h>
#include <random>

namespace voip {

	//һ��ͨ���Ĺ��캯��,
	SingleCall::SingleCall(unsigned int src_addr,
		unsigned short int src_port,
		unsigned int dst_addr,
		unsigned short int dst_port,
		unsigned short int id,
		unsigned int timestamp,
		unsigned int ssrc,
		unsigned short int seq_num,
		EncoderFactory* encoder_factory_ptr,
		GeneratorFactory* generator_factory_ptr,
		Consumer* consumer_ptr)
	{
		m_accumulated_step_time = 0;   //�Ѿ������ʱ��
		m_remaining_time = 0;        //ʣ��ʱ��
		m_encoder_ptr = encoder_factory_ptr->CreateEncoder();   //����һ���������������ض���ָ��
		m_generator_ptr = generator_factory_ptr->CreateGenerator();  //����һ�������ߣ������ض���ָ��
		m_consumer_ptr = consumer_ptr;
		m_line_data.m_rtp_data_size = m_encoder_ptr->GetPacketSize();     //��ȡRTP�����ݴ�С

		// form rtp header
		m_rtp_header.payload = m_encoder_ptr->GetRtpPayloadType();
		m_rtp_header.timestamp = timestamp;
		m_rtp_header.ssrc = ssrc;
		m_rtp_header.seq_num = seq_num;
		m_rtp_header.version = 2;
		m_rtp_header.cc = 0;
		m_rtp_header.x = 0;
		m_rtp_header.p = 0;
		m_rtp_header.m = 0;

		// form udp header
		m_udp_header.srcPort = src_port;
		m_udp_header.dstPort = dst_port;
		m_udp_header.totalLen = m_line_data.m_rtp_data_size + rtp_header_size + udp_header_size;
		m_udp_header.checkSum = 0;    // set to zero, willbe updated when writing to buffer

									  // form ipv4 header
		m_ipv4_header.hdr_len = 0x5;
		m_ipv4_header.version = 0x4;
		m_ipv4_header.serviceType = 0xb8;
		m_ipv4_header.fragment = 0;
		m_ipv4_header.ttl = 128;
		m_ipv4_header.srcAddr = src_addr;
		m_ipv4_header.dstAddr = dst_addr;
		m_ipv4_header.totalLen = m_udp_header.totalLen + ipv4_header_size;
		m_ipv4_header.protocol = 17;
		m_ipv4_header.id = id;
		m_ipv4_header.checkSum = 0;    // set to zero, will be updated when writing to buffer

	 // form pseudo ipv4 header
		m_pseudo_ipv4_header.srcAddr = m_ipv4_header.srcAddr;
		m_pseudo_ipv4_header.dstAddr = m_ipv4_header.dstAddr;
		m_pseudo_ipv4_header.protocol = m_ipv4_header.protocol;
		m_pseudo_ipv4_header.dataLen = m_udp_header.totalLen;

		// form ethernet header
		*((unsigned int*)(m_eth_header.src_mac + 2)) = htonl(src_addr);
		*((unsigned int*)(m_eth_header.dst_mac + 2)) = htonl(dst_addr);
		m_eth_header.eth_type = 0x0800;
	}

	SingleCall::~SingleCall()
	{
		//���������encoder������ô�ͷ�
		if (m_encoder_ptr)
		{
			delete m_encoder_ptr;
			m_encoder_ptr = nullptr;
		}
		//���������generator������ô�ͷ�
		if (m_generator_ptr)
		{
			delete m_generator_ptr;
			m_generator_ptr = nullptr;
		}
	}

	//����   step_durationӦ���Ǳ��η���ĳ���ʱ��
	void SingleCall::Step(unsigned short int step_duration)
	{
		//�����ܹ���Ҫ�ķ���ʱ�䣬m_accumulated_step_time = 0+ step_duration
		m_accumulated_step_time += step_duration;

		//���ʣ���ʱ�仹��һ������ѹ�����
		//encoder��duration  Ӧ����ѹ���ĳ���ʱ��
		while (m_accumulated_step_time >= m_encoder_ptr->GetPacketDuration())
		{
			//generator ����PCM���ݣ�PCM�����m_pcm_data_ptr��ַ����PCM�Ĵ�СΪm_line_data.m_rtp_data_size�Ĵ�С
			if (!m_generator_ptr->Generate(m_pcm_data_ptr, m_line_data.m_rtp_data_size))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_generator_ptr->Generate() failed" << std::endl;
				return;
			}
			//��PCM����ѹ������,ѹ��������ݱ�����m_rtp_data_ptr��
			if (!m_encoder_ptr->Encode(m_pcm_data_ptr, m_line_data.m_rtp_data_ptr))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_encoder_ptr->Encode() failed" << std::endl;
				return;
			}
			//����ǰRTP������д�뵽m_line_data.m_rtp_hdr_ptr��
			if (false == m_rtp_header.WriteToBuffer(m_line_data.m_rtp_hdr_ptr))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_rtp_header.WriteToBuffer() failed" << std::endl;
				return;
			}
			// update udp length if necessary
			// m_udp_header.tot_len =
			//����UDP����У���
			if (false == m_udp_header.UpdateChecksumWriteToBuffer(m_line_data.m_udp_hdr_ptr, m_line_data.m_rtp_hdr_ptr, m_pseudo_ipv4_header))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_udp_header.UpdateChecksumWriteToBuffer() failed" << std::endl;
				return;
			}
			//���¼���IPV4��У���
			if (false == m_ipv4_header.UpdateChecksumWriteToBuffer(m_line_data.m_ipv4_hdr_ptr))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_ipv4_header.UpdateChecksumWriteToBuffer() failed" << std::endl;
				return;
			}
			//д����̫֡ͷ��
			if (false == m_eth_header.WriteToBuffer(m_line_data.m_eth_hdr_ptr))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_eth_header.WriteToBuffer() failed" << std::endl;
				return;
			}
			//��������̫֡���� ���͸�Consumer,Ȼ����consume��������PACP
			m_consumer_ptr->Consume(m_line_data.m_line_data, m_line_data.LineDataSize());

			//m_encoder_ptr->GetPacketDuration()ָ�������Զ೤ʱ�������ѹ����Ĭ��20MS,����ʣ��ķ���ʱ��
			// update necessary fields for the next iteration / step
			m_accumulated_step_time -= m_encoder_ptr->GetPacketDuration();
			//rtp���������
			m_rtp_header.seq_num++;
			//RTP��ʱ����ǵ���������������
			m_rtp_header.timestamp += m_encoder_ptr->GetPacketSize();
			// ipv4��id����
			m_ipv4_header.id++;
		}
	}

	//call �Ĺ��캯��������duration(����ʱ��)����λӦ����Ms
	Call::Call(unsigned int duration) : m_duration(duration * 1000)
	{
	}

	//Call�� ��������
	bool Call::Step(unsigned int step_duration)
	{
		//һ��ʼ�϶���ʱ��
		bool still_has_time = true;
		//�����ǰ��ʱ��
		if (m_duration < step_duration)
		{
			// change step duration to whatever time call has
			step_duration = m_duration;
			still_has_time = false;
		}

		for (auto& cl : m_singlecall_ptr)
		{
			cl->Step(step_duration);
		}

		m_duration -= step_duration;
		return still_has_time;
	}
	

	//���캯��
	MirrorCall::MirrorCall(unsigned int src_ip, unsigned int dst_ip, unsigned int duration,
		EncoderFactory* encoder_factory_ptr,
		GeneratorFactory* generator_factory_ptr,
		Consumer* consumer_ptr) : Call(duration)
	{
		int no_of_call_legs = 2;
		//id��ƫ���������ֻ�ܲ������ٸ� 
		unsigned short id_offset = USHRT_MAX / no_of_call_legs;
		unsigned short int id = 1;

		// form a seed
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

		// introduce generator
		std::minstd_rand generator(seed);

		std::uniform_int_distribution<unsigned int> uint_distribution(0, UINT_MAX);
		std::uniform_int_distribution<unsigned short int> usint_distribution(0, USHRT_MAX);

		unsigned short int src_port = 32514;
		unsigned short int dst_port = 32514;

		//ʱ�����������
		unsigned int timestamp = uint_distribution(generator);
		//ssrc ��������
		unsigned int ssrc = uint_distribution(generator);
		//seq_num ��������
		unsigned short int seq_num = usint_distribution(generator);

		//����һ��callleg����
		// src Դ
		auto src_call_leg = std::make_unique<SingleCall>(src_ip, src_port, dst_ip, dst_port, id, timestamp, ssrc, seq_num, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
		//Ϊʲô��Ҫ����������
		m_singlecall_ptr.push_back(std::move(src_call_leg));

		id += id_offset;
		timestamp = uint_distribution(generator);
		ssrc = uint_distribution(generator);
		seq_num = usint_distribution(generator);
		//dst Ŀ�� 
		auto dst_call_leg = std::make_unique<SingleCall>(dst_ip, dst_port, src_ip, src_port, id, timestamp, ssrc, seq_num, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
		//m_call_leg_ptr_vector ���������ˣ�
		m_singlecall_ptr.push_back(std::move(dst_call_leg));

		std::clog << "-------------------------------- MIRROR CALL CREATION -------------------------------------" << std::endl;
		std::clog << "HOST : " << std::hex << src_ip << std::dec << ":" << src_port << " <--> " << std::hex << dst_ip << std::dec << ":" << dst_port << std::endl;
		std::clog << "NET  : " << std::hex << htonl(src_ip) << std::dec << ":" << htons(src_port) << " <--> " << std::hex << htonl(dst_ip) << std::dec << ":" << htons(dst_port) << std::endl;
	}


	//duration ��call �ĳ���ʱ��ô��
	std::unique_ptr<Call> MirrorCallFactory::CreateCall(unsigned int duration,
		EncoderFactory* encoder_factory_ptr,
		GeneratorFactory* generator_factory_ptr,
		Consumer* consumer_ptr)
	{
		//IP_pool��ʲô
		unsigned int src_ip = m_ip_pool++;
		unsigned int dst_ip = m_ip_pool++;
		//����һ������
		return std::make_unique<MirrorCall>(src_ip, dst_ip, duration, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
	}


}