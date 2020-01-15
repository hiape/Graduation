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

	//RTP��ͷ��
	struct RtpHeader{
		uchar cc : 4;   //csrc��������ռ4λ
		uchar x : 1;    //��չ��־λ��ռһλ
		uchar p : 1;    //����־��ռһλ
		uchar version : 2;  //RTPЭ��汾�ţ�ռ��λ
		uchar payload : 7; //���ص����� ��7λ
		uchar m : 1;  //��ǣ�ռһλ
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

	//αIPV4��ͷ��
	struct PseudoIpv4Header
	{
		unsigned int srcAddr;            //ԴIP��ַ
		unsigned int dstAddr;            //Ŀ��IP��ַ
		unsigned char zeroPad;           /**< zero pad */
		unsigned char protocol;           //Э������
		unsigned short int dataLen;      //���ݳ���

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


	//IPV4�ı��ĸ�ʽ
	struct Ipv4Header
	{
		unsigned char hdr_len : 4;        //IPV4 �ײ������ֶΣ�4Bit
		unsigned char version : 4;        //�汾  4bit
		unsigned char serviceType;		  //��������
		unsigned short int totalLen;      //�ܳ���   16bit
		unsigned short int id;            // ��ʶ��ÿ��IP����һ����������id����
		unsigned short int fragment;    //��Ƭ��Ŀǰֻ��ֻ����λ�����壬���0��ʾ���һ�����ݣ�����Ƭ
		unsigned char ttl;              //����ʱ��   8bit
		unsigned char protocol;         //Э������   8bit
		unsigned short int checkSum;    //�ײ�У��ͣ�  16λ
		unsigned int srcAddr;          //Դ��ַ,    32bit
		unsigned int dstAddr;          //Ŀ�ĵ�ַ��  32bit

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

	//UDP����ͷ����Ϣ��װ
	struct UdpHeader {
		unsigned short int srcPort;   //Դ�˿�
		unsigned short int dstPort;   //Ŀ�Ķ˿�
		unsigned short int totalLen;   //udp���ܳ���
		unsigned short int checkSum;   //У���


		bool ReadFromBuffer(const unsigned char* buffer_ptr);

		bool UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr, const unsigned char* udp_data_ptr, const PseudoIpv4Header& pseudo_ipv4_header);

		bool operator ==(const UdpHeader& rhs) const;

		bool WriteToBuffer(unsigned char* buffer_ptr);

		void Display() const;
	};

	const unsigned short int udp_header_size = sizeof(UdpHeader);    /**< size of udp header */

	//��̫֡��ʽ��װ
	struct EthHeader
	{
		unsigned char dst_mac[6];    //Ŀ��MAC��ַ
		unsigned char src_mac[6];    //ԴMAC��ַ
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

	//һ�����ݵ���󳤶�65536
#define MAX_LINE_DATA_SIZE 65536    /**< maximum line packet size */

	struct LineData
	{
		unsigned char m_line_data[MAX_LINE_DATA_SIZE];    /**< pointer to start of line data */
		unsigned char* m_eth_hdr_ptr;    /**< pointer to start of ethernet header */
		unsigned char* m_ipv4_hdr_ptr;    /**< pointer to start of internet header */
		unsigned char* m_udp_hdr_ptr;    /**< pointer to start of udp header */
		unsigned char* m_rtp_hdr_ptr;    /**< pointer to start of rtp header */
		unsigned char* m_rtp_data_ptr;    /**< pointer to start of rtp data */
		unsigned short int m_rtp_data_size;    //rtp���ݳ���
								
		//m_line_data��������Ϊm_eth_hdr_ptr��ʼ��
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