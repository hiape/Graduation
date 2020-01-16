#pragma once
#include "consumer.h"
#include "configure.h"
#include "encoder.h"
#include "generator.h"
#include <memory>
#include <vector>

namespace voip {

	//单独的一次通话
	class SingleCall {
	private:
		RtpHeader m_rtp_header;    // rtp Í·²¿
		UdpHeader m_udp_header;    //udp Í·²¿
		Ipv4Header m_ipv4_header;    //ipv4 Í·²¿
		EthHeader m_eth_header;    // ÒÔÌ«Ö¡ Í·²¿
		PseudoIpv4Header m_pseudo_ipv4_header;    //Î±IPV4µÄÍ·²¿

		uint m_remaining_time;    //Ê£ÓàµÄ·ÂÕæÊ±¼ä
		uint m_accumulated_step_time;    //ÉÐÎ´´¦ÀíµÄstepÊ±¼ä

		Encoder* m_encoder_ptr;    //±àÂëÆ÷
		Generator* m_generator_ptr;    //   WAV²¨ÐÎÎÄ¼þµÄ²úÉúµÄÆ÷
		Consumer* m_consumer_ptr;      //Consumer

		short int m_pcm_data_ptr[MAX_PCM_DATA_SIZE];   //rtpµÄ×î´ó×Ö½ÚÊý
		LineData m_line_data;    //linedataÊÇ×îÖÕµÄ´ý´¦ÀíÊý¾Ý

	public:
		//¹¹Ôìº¯Êý
		SingleCall(uint srcAddr,
			unsigned short int src_port,
			unsigned int dst_addr,
			unsigned short int dst_port,
			unsigned short int id,
			unsigned int timestamp,
			unsigned int ssrc,
			unsigned short int seq_num,
			EncoderFactory* encoder_factory_ptr,
			GeneratorFactory* generator_factory_ptr,
			Consumer* consumer_ptr);
	
		//Îö¹¹
		~SingleCall();

		//Æô¶¯º¯Êý
		void Step(unsigned short int step_durtion);    /**< Make a step in simulation */
	};

	//Í¨»°·½·¨½Ó¿Ú
	class Call
	{
	protected:
		Call(unsigned int duration);
		std::vector<std::unique_ptr<SingleCall>> m_singlecall_ptr;    //Ê¹ÓÃÈÝÆ÷±£´æÍ¨»°
		unsigned int m_duration;    //³ÖÐøÊ±¼ä

	public:
		//
		Call() = delete;
		virtual ~Call() = default;

		/**
		* @brief Function to step simulation
		*
		* @param step_duration INPUT duration to simulate a call
		* @return success of operation
		*/
		//step_duration ÊÇÄ£ÄâµÄºô½ÐÊ±¼ä
		virtual bool Step(unsigned int step_duration);
	};

	//¾µÏñÍ¨ÐÅ·½·¨
	class MirrorCall :public Call
	{
	public:
		MirrorCall(unsigned int src_ip, unsigned int dst_ip, unsigned int duration,
			EncoderFactory* encoder_factory_ptr,
			GeneratorFactory* generator_factory_ptr,
			Consumer* consumer_ptr);

		~MirrorCall() = default;
	};

	//Í¨»°µÄ³éÏó½Ó¿Ú
	class CallFactory
	{
	public:

		CallFactory() {}

		virtual ~CallFactory() = default;

		virtual std::unique_ptr<Call> CreateCall(unsigned int duration,
			EncoderFactory* encoder_factory_ptr,
			GeneratorFactory* generator_factory_ptr,
			Consumer* consumer_ptr) = 0;
	};

	class MirrorCallFactory : public CallFactory
	{
	private:
		unsigned int m_ip_pool;  //ip³ØÊÇÊ²Ã´??
	public:

		MirrorCallFactory(unsigned int start_ip) : m_ip_pool(start_ip)
		{
		}

		virtual ~MirrorCallFactory() = default;

		/**
		* @brief Implementation of sinusoidal generator creation
		*
		* @return SinusoidalGeneratorType is created and returned to calling object
		* @see GeneratorType()
		* @see ZeroGeneratorType()
		* @see SingleToneGeneratorType()
		*/
		virtual std::unique_ptr<Call> CreateCall(unsigned int duration,
			EncoderFactory* encoder_factory_ptr,
			GeneratorFactory* generator_factory_ptr,
			Consumer* consumer_ptr);
	};

}