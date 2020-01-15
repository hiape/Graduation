#pragma once
#include "configure.h"

namespace voip
{

	/**
	* @brief Method that will calculate ones complement 16 bit summation .
	*
	* 16 bit ones complement addition is performed on data_ptr.
	* If data size is not multiple of 2, zero is padded to higher bit of last character.
	* @param data_ptr INPUT data array that summation will be handled on
	* @param data_size INPUT size of data array
	* @return ones complement 16 bit summation result
	*/
	unsigned short int OnesComplementShortSummation(const unsigned char* data_ptr, unsigned short int data_size);

	//RTP的头部
	struct RtpHeader{
		uchar cc : 4;   //csrc计数器，占4位
		uchar x : 1;    //扩展标志位，占一位
		uchar p : 1;    //填充标志，占一位
		uchar version : 2;  //RTP协议版本号，占两位
		uchar payload : 7; //负载的类型 ，7位
		uchar m : 1;  //标记，占一位
		unsigned short int seq_num;   
		uint  timestamp;
		uint ssrc;
		
	//function
		bool ReadFromBuffer(const unsigned char* buffer_ptr);
		bool WriteToBuffer(unsigned char* buffer_ptr) const;
		bool operator ==(const RtpHeader& rhs) const;
	//	void Display() const;
	};

	const unsigned short int rtp_header_size = sizeof(RtpHeader);    /**< size of rtp header */

	//伪IPV4的头部
	struct PseudoIpv4Header
	{
		unsigned int srcAddr;            //源IP地址
		unsigned int dstAddr;            //目的IP地址
		unsigned char zeroPad;           /**< zero pad */
		unsigned char protocol;           //协议类型
		unsigned short int dataLen;      //数据长度

		PseudoIpv4Header() : srcAddr(0), dstAddr(0), zeroPad(0), protocol(0), dataLen(0) {}

		/**
		* @brief Write contents to raw line buffer
		*
		* Structure data is converted into network byte order and written into buffer.
		* Line buffer is expected to be pointing 12 bytes to write into.
		* @param buffer_ptr OUTPUT line buffer to write to
		* @return success or failure due to buffer write
		* @see ReadFromBuffer()
		*/
		bool WriteToBuffer(unsigned char* buffer_ptr) const;

		/**
		* @brief Display contents of pseudo Ipv4 header
		*/
		void Display() const;
	};

	const unsigned short int pseudo_ipv4_header_size = sizeof(PseudoIpv4Header);


	//IPV4的报文格式
	struct Ipv4Header
	{
		unsigned char hdr_len : 4;        //IPV4 首部长度字段，4Bit
		unsigned char version : 4;        //版本  4bit
		unsigned char serviceType;		  //服务类型
		unsigned short int totalLen;      //总长度   16bit
		unsigned short int id;            // 标识，每个IP包有一个计数器，id递增
		unsigned short int fragment;    //分片，目前只有只有两位有意义，如果0表示最后一个数据，不分片
		unsigned char ttl;              //生存时间   8bit
		unsigned char protocol;         //协议类型   8bit
		unsigned short int checkSum;    //首部校验和，  16位
		unsigned int srcAddr;          //源地址,    32bit
		unsigned int dstAddr;          //目的地址，  32bit

		bool ReadFromBuffer(const unsigned char* buffer_ptr);

		/**
		* @brief Update checksum and write contents to raw line buffer
		*
		* Structure data is converted into network byte order and written into buffer.
		* Line buffer is expected to be pointing 20 bytes to write into. Checksum is calculated before operation
		* @param buffer_ptr OUTPUT line buffer to write to
		* @return success or failure due to buffer write
		* @see ReadFromBuffer()
		*/
		bool UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr);

		/**
		* @brief Write contents to raw line buffer
		*
		* Structure data is converted into network byte order and written into buffer.
		* Line buffer is expected to be pointing 20 bytes to write into. It is assumed that checksum
		* field is calculated and set accordingly.
		* @param buffer_ptr OUTPUT line buffer to write to
		* @return success or failure due to buffer write
		* @see ReadFromBuffer()
		*/
		bool WriteToBuffer(unsigned char* buffer_ptr);

		/**
		* @brief overloading equivalence operator
		*
		* Since there is not a dynamic allocated data, std::memcmp is used in comparison
		* @param rhs other Ipv4HeaderType to compare with
		* @return boolean result of equivalence check
		*/
		bool operator ==(const Ipv4Header& rhs) const;

		/**
		* @brief Display contents of Ipv4 header
		*/
		void Display() const;
	};

	const unsigned short int ipv4_header_size = sizeof(Ipv4Header);    /**< size of ipv4 header */

	//UDP报文头部信息封装
	struct UdpHeader {
		unsigned short int srcPort;   //源端口
		unsigned short int dstPort;   //目的端口
		unsigned short int totalLen;   //udp包总长度
		unsigned short int checkSum;   //校验和


		bool ReadFromBuffer(const unsigned char* buffer_ptr);

		bool UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr, const unsigned char* udp_data_ptr, const PseudoIpv4Header& pseudo_ipv4_header);

		bool operator ==(const UdpHeader& rhs) const;

		bool WriteToBuffer(unsigned char* buffer_ptr);

		void Display() const;
	};

	const unsigned short int udp_header_size = sizeof(UdpHeader);    /**< size of udp header */

	//以太帧格式封装
	struct EthHeader
	{
		unsigned char dst_mac[6];    //目的MAC地址
		unsigned char src_mac[6];    //源MAC地址
		unsigned short int eth_type;     /**< Type of payload */

										 /**
										 * @brief Read contents from raw line buffer
										 *
										 * Ethernet header data is read from line buffer and converted into host byte order.
										 * Line buffer is expected to be pointing 14 bytes of Ethernet header.
										 * @param buffer_ptr INPUT line buffer to read from
										 * @return success or failure due to buffer read
										 * @see WriteToBuffer()
										 */
		bool ReadFromBuffer(const unsigned char* buffer_ptr);

		/**
		* @brief Write contents to raw line buffer
		*
		* Structure data is converted into network byte order and written into buffer.
		* Line buffer is expected to be pointing 14 bytes to write into.
		* @param buffer_ptr OUTPUT line buffer to write to
		* @return success or failure due to buffer write
		* @see ReadFromBuffer()
		*/
		bool WriteToBuffer(unsigned char* buffer_ptr);

		/**
		* @brief Display contents of Ethernet header
		*/
		void Display() const;
	};

	const unsigned short int eth_header_size = sizeof(EthHeader);    /**< size of ethernet header */

	//一行数据的最大长度65536
#define MAX_LINE_DATA_SIZE 65536    /**< maximum line packet size */

	struct LineData
	{
		unsigned char m_line_data[MAX_LINE_DATA_SIZE];    /**< pointer to start of line data */
		unsigned char* m_eth_hdr_ptr;    /**< pointer to start of ethernet header */
		unsigned char* m_ipv4_hdr_ptr;    /**< pointer to start of internet header */
		unsigned char* m_udp_hdr_ptr;    /**< pointer to start of udp header */
		unsigned char* m_rtp_hdr_ptr;    /**< pointer to start of rtp header */
		unsigned char* m_rtp_data_ptr;    /**< pointer to start of rtp data */
		unsigned short int m_rtp_data_size;    //rtp数据长度
								
		//m_line_data的内容作为m_eth_hdr_ptr开始，
		LineData()
		{
			m_eth_hdr_ptr = m_line_data;
			m_ipv4_hdr_ptr = m_eth_hdr_ptr + eth_header_size;
			m_udp_hdr_ptr = m_ipv4_hdr_ptr + ipv4_header_size;
			m_rtp_hdr_ptr = m_udp_hdr_ptr + udp_header_size;
			m_rtp_data_ptr = m_rtp_hdr_ptr + rtp_header_size;
			m_rtp_data_size = 0;
		}

		unsigned int LineDataSize()
		{
			return (eth_header_size + ipv4_header_size + udp_header_size + rtp_header_size + m_rtp_data_size);
		}
	};
}