#include "voipcall.h"
#include "generator.h"

#include <netinet/in.h>
#include <iostream>
#include <chrono>
#include <limits.h>
#include <random>

namespace voip {

	//一次通话的构造函数,
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
		m_accumulated_step_time = 0;   //已经仿真的时间
		m_remaining_time = 0;        //剩余时间
		m_encoder_ptr = encoder_factory_ptr->CreateEncoder();   //创建一个编码器，并返回对象指针
		m_generator_ptr = generator_factory_ptr->CreateGenerator();  //创建一个产生者，并返回对象指针
		m_consumer_ptr = consumer_ptr;
		m_line_data.m_rtp_data_size = m_encoder_ptr->GetPacketSize();     //获取RTP的数据大小

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
		//如果创建了encoder对象，那么释放
		if (m_encoder_ptr)
		{
			delete m_encoder_ptr;
			m_encoder_ptr = nullptr;
		}
		//如果创建了generator对象，那么释放
		if (m_generator_ptr)
		{
			delete m_generator_ptr;
			m_generator_ptr = nullptr;
		}
	}

	//仿真   step_duration应该是本次仿真的持续时间
	void SingleCall::Step(unsigned short int step_duration)
	{
		//计算总共需要的仿真时间，m_accumulated_step_time = 0+ step_duration
		m_accumulated_step_time += step_duration;

		//如果剩余的时间还够一个数据压缩打包
		//encoder的duration  应该是压缩的持续时间
		while (m_accumulated_step_time >= m_encoder_ptr->GetPacketDuration())
		{
			//generator 产生PCM数据，PCM存放在m_pcm_data_ptr地址处，PCM的大小为m_line_data.m_rtp_data_size的大小
			if (!m_generator_ptr->Generate(m_pcm_data_ptr, m_line_data.m_rtp_data_size))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_generator_ptr->Generate() failed" << std::endl;
				return;
			}
			//对PCM进行压缩处理,压缩后的数据保存在m_rtp_data_ptr中
			if (!m_encoder_ptr->Encode(m_pcm_data_ptr, m_line_data.m_rtp_data_ptr))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_encoder_ptr->Encode() failed" << std::endl;
				return;
			}
			//将当前RTP中内容写入到m_line_data.m_rtp_hdr_ptr中
			if (false == m_rtp_header.WriteToBuffer(m_line_data.m_rtp_hdr_ptr))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_rtp_header.WriteToBuffer() failed" << std::endl;
				return;
			}
			// update udp length if necessary
			// m_udp_header.tot_len =
			//重新UDP计算校验和
			if (false == m_udp_header.UpdateChecksumWriteToBuffer(m_line_data.m_udp_hdr_ptr, m_line_data.m_rtp_hdr_ptr, m_pseudo_ipv4_header))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_udp_header.UpdateChecksumWriteToBuffer() failed" << std::endl;
				return;
			}
			//重新计算IPV4的校验和
			if (false == m_ipv4_header.UpdateChecksumWriteToBuffer(m_line_data.m_ipv4_hdr_ptr))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_ipv4_header.UpdateChecksumWriteToBuffer() failed" << std::endl;
				return;
			}
			//写入以太帧头部
			if (false == m_eth_header.WriteToBuffer(m_line_data.m_eth_hdr_ptr))
			{
				std::cerr << __FILE__ << " " << __LINE__ << "m_eth_header.WriteToBuffer() failed" << std::endl;
				return;
			}
			//将最后的以太帧数据 输送给Consumer,然后由consume方法产生PACP
			m_consumer_ptr->Consume(m_line_data.m_line_data, m_line_data.LineDataSize());

			//m_encoder_ptr->GetPacketDuration()指编码器对多长时间的数据压缩，默认20MS,计算剩余的仿真时间
			// update necessary fields for the next iteration / step
			m_accumulated_step_time -= m_encoder_ptr->GetPacketDuration();
			//rtp的序号增加
			m_rtp_header.seq_num++;
			//RTP的时间戳是递增采样的样本数
			m_rtp_header.timestamp += m_encoder_ptr->GetPacketSize();
			// ipv4的id增加
			m_ipv4_header.id++;
		}
	}

	//call 的构造函数，传入duration(仿真时间)，单位应该是Ms
	Call::Call(unsigned int duration) : m_duration(duration * 1000)
	{
	}

	//Call的 启动函数
	bool Call::Step(unsigned int step_duration)
	{
		//一开始肯定有时间
		bool still_has_time = true;
		//如果当前的时间
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
	

	//构造函数
	MirrorCall::MirrorCall(unsigned int src_ip, unsigned int dst_ip, unsigned int duration,
		EncoderFactory* encoder_factory_ptr,
		GeneratorFactory* generator_factory_ptr,
		Consumer* consumer_ptr) : Call(duration)
	{
		int no_of_call_legs = 2;
		//id的偏移量，最多只能产生多少个 
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

		//时间戳用在哪里
		unsigned int timestamp = uint_distribution(generator);
		//ssrc 用在哪里
		unsigned int ssrc = uint_distribution(generator);
		//seq_num 用在哪里
		unsigned short int seq_num = usint_distribution(generator);

		//创建一个callleg对象
		// src 源
		auto src_call_leg = std::make_unique<SingleCall>(src_ip, src_port, dst_ip, dst_port, id, timestamp, ssrc, seq_num, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
		//为什么需要放在容器里
		m_singlecall_ptr.push_back(std::move(src_call_leg));

		id += id_offset;
		timestamp = uint_distribution(generator);
		ssrc = uint_distribution(generator);
		seq_num = usint_distribution(generator);
		//dst 目的 
		auto dst_call_leg = std::make_unique<SingleCall>(dst_ip, dst_port, src_ip, src_port, id, timestamp, ssrc, seq_num, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
		//m_call_leg_ptr_vector 用来干嘛了？
		m_singlecall_ptr.push_back(std::move(dst_call_leg));

		std::clog << "-------------------------------- MIRROR CALL CREATION -------------------------------------" << std::endl;
		std::clog << "HOST : " << std::hex << src_ip << std::dec << ":" << src_port << " <--> " << std::hex << dst_ip << std::dec << ":" << dst_port << std::endl;
		std::clog << "NET  : " << std::hex << htonl(src_ip) << std::dec << ":" << htons(src_port) << " <--> " << std::hex << htonl(dst_ip) << std::dec << ":" << htons(dst_port) << std::endl;
	}


	//duration 是call 的持续时间么？
	std::unique_ptr<Call> MirrorCallFactory::CreateCall(unsigned int duration,
		EncoderFactory* encoder_factory_ptr,
		GeneratorFactory* generator_factory_ptr,
		Consumer* consumer_ptr)
	{
		//IP_pool是什么
		unsigned int src_ip = m_ip_pool++;
		unsigned int dst_ip = m_ip_pool++;
		//创建一个对象
		return std::make_unique<MirrorCall>(src_ip, dst_ip, duration, encoder_factory_ptr, generator_factory_ptr, consumer_ptr);
	}


}