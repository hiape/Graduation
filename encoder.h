#pragma once
#include "configure.h"

#include <string>

namespace voip{

	//±àÂëÆ÷
	class Encoder{
	private:
		uint m_packetDuration;   //³ÖÐøÊ±¼ä£¬µ¥Î» ns
		uint m_packetSize;      //´ò°üµÄ´óÐ¡,µ¥Î» Byte
		uint m_fs;              //²ÉÑùµÄÆµÂÊ,µ¥Î» Hz
		uint m_bitSample;		//²ÉÑùµÄÎ»Êý,µ¥Î» Bit
	public:
		Encoder(uint duration=2000,uint packetSize = 160,uint fs=8000 ,uint bitSample = 8):m_packetDuration(duration),
				m_packetSize(packetSize),
				m_fs(fs),
				m_bitSample(bitSample)
		{}

		virtual ~Encoder() {}

		virtual bool Encode(const short int* pcm_data_ptr, uchar* encoded_data_ptr) = 0;
		//»ñÈ¡¶ÔÓ¦RTPµÄ ÒôÆµÑ¹ËõÀàÐÍ
		virtual uchar GetRtpPayloadType() const = 0;
		//»ñÈ¡±àÂëÆ÷Ò»´Î±àÂëµÄ²ÉÑùÊý£¬ ÐèÒª¶àÉÙ¸ö²ÉÑùÑù±¾
		virtual unsigned short int GetPacketSize() const = 0;
		//·µ»ØÄ¬ÈÏµÄ´ò°ü¼ä¸ô
		virtual unsigned short int GetPacketDuration() const { return m_packetDuration; }

	};

	//  G.711A  ±àÂë·½Ê½
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

	//编码工厂
	class EncoderFactory
	{
	private:

	public:
		//¹¹Ôìº¯Êý
		EncoderFactory() {}

		//Îö¹¹º¯Êý
		virtual ~EncoderFactory() {}

		//´´½¨±àÂëÆ÷
		virtual Encoder* CreateEncoder() const = 0;

		virtual const std::string GetEncoderName() const = 0;
	};

	class G711aEncoderFactory : public EncoderFactory
	{
	private:

	public:
		/**
		* @brief Default constructor, does not perform any specific operation
		*/
		G711aEncoderFactory() { }

		/**
		* @brief Default destructor, does not perform any specific operation
		*/
		virtual ~G711aEncoderFactory() {}

		/**
		* @brief Implementation of encoder generation
		*
		* @return G711aEncoderType is created and returned to calling object
		* @see EncoderType()
		* @see G711aEncoderType()
		*/
		virtual Encoder* CreateEncoder() const { return new G711aEncoder(); }

		virtual const std::string GetEncoderName()  const final  { return std::string("G711a");}
	};

	class G711uEncoderFactory : public EncoderFactory
	{
	private:

	public:
		/**
		* @brief Default constructor, does not perform any specific operation
		*/
		G711uEncoderFactory() {}

		/**
		* @brief Default destructor, does not perform any specific operation
		*/
		virtual ~G711uEncoderFactory() {}

		/**
		* @brief Implementation of encoder generation
		*
		* @return G711uEncoderType is created and returned to calling object
		* @see EncoderType()
		* @see G711uEncoderType()
		*/
		virtual Encoder* CreateEncoder() const { return new G711uEncoder(); }

		virtual const std::string GetEncoderName()  const  final { return std::string("G711u");}
	};
}