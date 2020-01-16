#pragma once
#include "configure.hpp"

namespace voip{

	//编码器
	class Encoder{
	private:
		uint m_packetDuration;   //持续时间，单位 ns
		uint m_packetSize;      //打包的大小,单位 Byte
		uint m_fs;              //采样的频率,单位 Hz
		uint m_bitSample;		//采样的位数,单位 Bit
	public:
		Encoder(uint duration=2000,uint packetSize = 160,uint fs=8000 ,uint bitSample = 8):m_packetDuration(duration),
				m_packetSize(packetSize),
				m_fs(fs),
				m_bitSample(bitSample)
		{}

		virtual ~Encoder() {}

		virtual bool Encode(const short int* pcm_data_ptr, uchar* encoded_data_ptr) = 0;
		//获取对应RTP 或者是 SRTP的 payload的类型
		virtual uchar GetRtpPayloadType() = 0;
		//获取打包的大小
		virtual unsigned short int GetPacketSize() const = 0;
		//返回默认的打包间隔
		virtual unsigned short int GetPacketDuration() const { return m_packetDuration; }

	};

	//  G.711A  编码方式
	class G711aEncoder :public Encoder{
	public:
		G711aEncoder() {}
		virtual ~G711aEncoder() {}

		virtual bool Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr);

		virtual unsigned char GetRtpPayloadType() const { return G711a_RTP_PAYLOAD_TYPE; }

		virtual unsigned short int GetPacketSize() const { return G711_PACKET_SIZE; }
	};


	class G711uEncoder :public  Encoder {
	public:
		G711uEncoder() {}
		virtual ~G711uEncoder() {}

		virtual bool Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr);

		virtual unsigned char GetRtpPayloadType() const { return G711u_RTP_PAYLOAD_TYPE; }

		virtual unsigned short int GetPacketSize() const { return G711_PACKET_SIZE; }
	};
}