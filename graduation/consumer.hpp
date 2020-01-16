#pragma once
#include "ipport.hpp"
#include "rowsocket.hpp"

#include <fstream>
#include <netinet/in.h>
#include <vector>

namespace voip {

	//消费者
	class Consumer
	{
	public:
		//默认构造函数
		Consumer() {}
		//虚析构函数
		virtual ~Consumer() {}

		//消费函数 ,数据地址 和数据的长度
		virtual bool Consume(const unsigned char* data_ptr, unsigned short int data_size) = 0;
	};

	//pcap文件消费者
	class PcapConsumer : public Consumer
	{
	private:

		//pcap的文件头格式
		struct PcapHdrType
		{
			unsigned int magic_number;    /**< An integer that takes major number. */
			unsigned short int version_major;    /**< major version number */
			unsigned short int version_minor;    /**< minor version number */
			unsigned int thiszone;
			unsigned int sigfigs;    /**< flags */
			unsigned int snaplen;
			unsigned int network;
		};

		/** @brief Types of Pcap packet header.
		*
		* Header information for classical .pcap packet
		*/
		//pcap的包头
		struct PcapPacHdrType
		{
			unsigned int ts_sec;    /**< packet time in sec */
			unsigned int ts_usec;    /**< packet time in micro sec */
			unsigned int incl_len;    /**< packet capture length */
			unsigned int orig_len;    /**< original packet length */
		};

	private:
		std::string m_file_name;    /**< file name for pcap file */
		std::fstream m_file_stream;    /**< file stream for pcap file */
		unsigned int m_file_size;    /**< An integer that shows size of pcap file. */
		const PcapHdrType m_pcap_file_header = { 0xa1b2c3d4, 2, 4, 0, 0, 65535, 1 };  //这是采用了一个默认值？  /**< pcap file haader for .pcap */

																					  /** @brief Generates file name
																					  @return Returns 1 if file is generated successfully.
																					  */
		void GenerateFileName();

	public:
		/**
		* @brief Constructor for initializing pcap file consumer
		*
		* @param file_name INPUT desired filename of pcap file. If already present, overriden
		*/
		PcapConsumer(std::string file_name = "");

		/**
		* @brief destructor, does close pcap file
		*/
		~PcapConsumer();

		/**
		* @brief Pcap file consumer for generated packets
		*
		* @param pcm_data_ptr INPUT pointer to data that will be consumed
		* @param data_size INPUT size, of data that will be consumed
		* @return indicates success of generation
		*/
		virtual bool Consume(const unsigned char* data_ptr, unsigned short int data_size);
	};
}
