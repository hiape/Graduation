#include "rowsocket.hpp"
#include <iostream>
#include <netinet/in.h>
#include <cstring>

namespace voip{

//计算校验和
unsigned short int OnesComplementShortSummation(const unsigned char* data_ptr, unsigned short int data_size)
{
	unsigned int sum = 0;

	for (unsigned short int i = 0; i < data_size / 2; i++)
	{
		sum += ntohs(*((unsigned short int*)data_ptr));
		data_ptr += 2;
	}

	if (data_size % 2)
		sum += *data_ptr;

	// sum carry
	sum = (sum >> 16) + (sum & 0x0000ffff);

	return (unsigned short int)sum;
}
//buffer_ptr  从一个缓冲区地址读内容到RTP中
bool RtpHeader::ReadFromBuffer(const unsigned char * buffer_ptr)
{
	//如果指针为空
	if (!buffer_ptr)
		return false;
	//内容的拷贝
	*this = *((const RtpHeader*)buffer_ptr);

	//从网络序转为主机序
	seq_num = ntohs(seq_num);
	timestamp = ntohl(timestamp);
	ssrc = ntohl(ssrc);
	return true;
}


//将RTP内容 写入到一个缓冲区中
bool RtpHeader::WriteToBuffer(unsigned char* buffer_ptr) const
{
	// check if buffer_ptr points to somewhere (hopefully) to 12 byte rtp header
	if (!buffer_ptr)
		return false;

	//创建一个备份
	RtpHeader dummy_rtp_header = *this;
	// correct seq_num, time stamp and ssrc from host byte order to network byte order
	dummy_rtp_header.seq_num = htons(dummy_rtp_header.seq_num);
	dummy_rtp_header.timestamp = htonl(dummy_rtp_header.timestamp);
	dummy_rtp_header.ssrc = htonl(dummy_rtp_header.ssrc);

	// write whole bunch
	*((RtpHeader*)buffer_ptr) = dummy_rtp_header;
	return true;
}

bool RtpHeader::operator==(const RtpHeader & rhs) const
{
	if (0 == std::memcmp(this, &rhs, sizeof(RtpHeader)))
		return true;
	else
		return false;
}


bool PseudoIpv4Header::WriteToBuffer(unsigned char* buffer_ptr) const
{

	// check if buffer_ptr points to somewhere (hopefully) to 12 byte pseudo ipv4 header
	if (NULL == buffer_ptr)
		return false;

	PseudoIpv4Header dummy_pseudo_ipv4_header;

	// correct seq_num, time stamp from host byte order to network byte order
	dummy_pseudo_ipv4_header.dataLen = htons(dataLen);
	dummy_pseudo_ipv4_header.protocol = protocol;
	dummy_pseudo_ipv4_header.srcAddr = htonl(srcAddr);
	dummy_pseudo_ipv4_header.dstAddr = htonl(dstAddr);

	// write whole bunch
	*((PseudoIpv4Header*)buffer_ptr) = dummy_pseudo_ipv4_header;

	return true;
}

void PseudoIpv4Header::Display() const
{
	std::cout << "------------------ Pseudo Ipv4 Header ---------------------" << std::endl;
	std::cout << "Source address         : " << std::hex << srcAddr << std::dec << std::endl;
	std::cout << "Destination address    : " << std::hex << dstAddr << std::dec << std::endl;
	std::cout << "Protocol               : " << protocol << std::endl;
	std::cout << "Data length            : " << dataLen << std::endl;
}



// *************************************** IPv4Header *********************************************

bool Ipv4Header::ReadFromBuffer(const unsigned char* buffer_ptr)
{
	// check if buffer_ptr points to somewhere (hopefully) to 20 byte ipv4 header
	if (NULL == buffer_ptr)
		return false;

	// read whole bunch
	*this = *((const Ipv4Header*)buffer_ptr);

	// correct seq_num, time stamp from network byte order to host byte order
	totalLen = ntohs(totalLen);
	id = ntohs(id);
	checkSum = ntohs(checkSum);
	srcAddr = ntohl(srcAddr);
	dstAddr = ntohl(dstAddr);

	return true;
}

bool Ipv4Header::WriteToBuffer(unsigned char* buffer_ptr)
{
	// check if buffer_ptr points to somewhere (hopefully) to 20 byte ipv4 header
	if (NULL == buffer_ptr)
		return false;

	Ipv4Header dummy_ipv4_header;

	// correct seq_num, time stamp from host byte order to network byte order
	dummy_ipv4_header.hdr_len = hdr_len;
	dummy_ipv4_header.version = version;
	dummy_ipv4_header.serviceType = serviceType;
	dummy_ipv4_header.totalLen = htons(totalLen);
	dummy_ipv4_header.id = htons(id);
	dummy_ipv4_header.fragment = htons(fragment);
	dummy_ipv4_header.ttl = ttl;
	dummy_ipv4_header.protocol = protocol;
	dummy_ipv4_header.checkSum = htons(checkSum);
	dummy_ipv4_header.srcAddr = htonl(srcAddr);
	dummy_ipv4_header.dstAddr = htonl(dstAddr);

	// write whole bunch
	*((Ipv4Header*)buffer_ptr) = dummy_ipv4_header;

	return true;
}

bool Ipv4Header::UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr)
{
	// check if buffer_ptr points to somewhere (hopefully) to 20 byte ipv4 header
	if (NULL == buffer_ptr)
		return false;

	//保存一个副本
	Ipv4Header dummy_ipv4_header;

	// correct seq_num, time stamp from host byte order to network byte order
	dummy_ipv4_header.hdr_len = hdr_len;
	dummy_ipv4_header.version = version;
	dummy_ipv4_header.serviceType = serviceType;
	dummy_ipv4_header.totalLen = htons(totalLen);
	dummy_ipv4_header.id = htons(id);
	dummy_ipv4_header.fragment = htons(fragment);
	dummy_ipv4_header.ttl = ttl;
	dummy_ipv4_header.protocol = protocol;
	dummy_ipv4_header.checkSum = htons(0);
	dummy_ipv4_header.srcAddr = htonl(srcAddr);
	dummy_ipv4_header.dstAddr = htonl(dstAddr);

	// write whole bunch
	*((Ipv4Header*)buffer_ptr) = dummy_ipv4_header;

	// 重新计算校验和
	checkSum = ~(OnesComplementShortSummation(buffer_ptr, ipv4_header_size));

	// modify checksum in buffer_ptr
	*((unsigned short int*)(buffer_ptr + 10)) = htons(checkSum);

	return true;
}

bool Ipv4Header::operator ==(const Ipv4Header& rhs) const
{
	if (0 == std::memcmp(this, &rhs, sizeof(Ipv4Header)))
		return true;
	else
		return false;
}

void Ipv4Header::Display() const
{
	std::cout << "------------------ Ipv4 Header ---------------------" << std::endl;
	std::cout << "Version                 : " << (unsigned int)version << std::endl;
	std::cout << "Header length           : " << (unsigned int)hdr_len << std::endl;
	std::cout << "Service type            : " << std::hex << (unsigned int)serviceType << std::dec << std::endl;
	std::cout << "Total length            : " << totalLen << std::endl;
	std::cout << "Sequence identification : " << std::hex << id << std::dec << std::endl;
	std::cout << "Fragment information    : " << std::hex << fragment << std::dec << std::endl;
	std::cout << "Time to live            : " << (unsigned int)ttl << std::endl;
	std::cout << "Protocol                : " << (unsigned int)protocol << std::endl;
	std::cout << "Header checksum         : " << std::hex << checkSum << std::dec << std::endl;
	std::cout << "Source address          : " << std::hex << srcAddr << std::dec << std::endl;
	std::cout << "Destination address     : " << std::hex << dstAddr << std::dec << std::endl;
}

bool CheckIpv4Checksum(const unsigned char* line_ipv4_header_ptr)
{
	unsigned short int sum_result = OnesComplementShortSummation(line_ipv4_header_ptr, ipv4_header_size);
	return (sum_result == 0xffff);
}


// *************************************** UdpHeader *********************************************
bool UdpHeader::ReadFromBuffer(const unsigned char* buffer_ptr)
{
	// check if buffer_ptr points to somewhere (hopefully) to 8 byte udp header
	if (NULL == buffer_ptr)
		return false;

	// read whole bunch
	*this = *((const UdpHeader*)buffer_ptr);

	// correct seq_num, time stamp from network byte order to host byte order
	srcPort = ntohs(srcPort);
	dstPort = ntohs(dstPort);
	totalLen = ntohs(totalLen);
	checkSum = ntohs(checkSum);

	return true;
}

bool UdpHeader::UpdateChecksumWriteToBuffer(unsigned char* buffer_ptr, const unsigned char* udp_data_ptr, const PseudoIpv4Header& pseudo_ipv4_header)
{
	// check if buffer_ptr points to somewhere (hopefully) to 8 byte udp header
	if (NULL == buffer_ptr)
		return false;

	unsigned char line_pseudo_ipv4_ptr[pseudo_ipv4_header_size];
	pseudo_ipv4_header.WriteToBuffer(line_pseudo_ipv4_ptr);

	unsigned int sum = OnesComplementShortSummation(line_pseudo_ipv4_ptr, pseudo_ipv4_header_size);

	sum += OnesComplementShortSummation(udp_data_ptr, (totalLen - udp_header_size));

	UdpHeader dummy_udp_header;
	// correct seq_num, time stamp from host byte order to network byte order
	dummy_udp_header.srcPort = htons(srcPort);
	dummy_udp_header.dstPort = htons(dstPort);
	dummy_udp_header.totalLen = htons(totalLen);
	dummy_udp_header.checkSum = htons(0);

	// write whole bunch
	*((UdpHeader*)buffer_ptr) = dummy_udp_header;

	// calculate checksum
	sum += OnesComplementShortSummation(buffer_ptr, udp_header_size);
	checkSum = ~(((sum >> 16) + (sum & 0x0000ffff)));

	// write it also into buffer_ptr
	*((unsigned short int*)(buffer_ptr + 6)) = htons(checkSum);

	return true;
}

bool UdpHeader::WriteToBuffer(unsigned char* buffer_ptr)
{
	// check if buffer_ptr points to somewhere (hopefully) to 8 byte udp header
	if (NULL == buffer_ptr)
		return false;

	UdpHeader dummy_udp_header;
	// correct seq_num, time stamp from host byte order to network byte order
	dummy_udp_header.srcPort = htons(srcPort);
	dummy_udp_header.dstPort = htons(dstPort);
	dummy_udp_header.totalLen = htons(totalLen);
	dummy_udp_header.checkSum = htons(checkSum);

	// write whole bunch
	*((UdpHeader*)buffer_ptr) = dummy_udp_header;

	return true;
}

bool UdpHeader::operator ==(const UdpHeader& rhs) const
{
	if (0 == std::memcmp(this, &rhs, sizeof(UdpHeader)))
		return true;
	else
		return false;
}

void UdpHeader::Display() const
{
	std::cout << "------------------ Udp Header ---------------------" << std::endl;
	std::cout << "Source port         : " << srcPort << std::endl;
	std::cout << "Destination port    : " << dstPort << std::endl;
	std::cout << "Packet length       : " << totalLen << std::endl;
	std::cout << "Packet checksum     : " << std::hex << checkSum << std::dec << std::endl;
}


bool CheckUdpChecksum(const unsigned char* line_udp_packet_ptr, const PseudoIpv4Header& pseudo_ipv4_header)
{
	// check if line_udp_packet_ptr points to somewhere (hopefully) at least 8 byte of udp packet
	if (NULL == line_udp_packet_ptr)
		return false;

	unsigned char line_pseudo_ipv4_ptr[pseudo_ipv4_header_size];
	pseudo_ipv4_header.WriteToBuffer(line_pseudo_ipv4_ptr);

	unsigned int sum = OnesComplementShortSummation(line_pseudo_ipv4_ptr, pseudo_ipv4_header_size);

	sum += OnesComplementShortSummation(line_udp_packet_ptr, ntohs(*((unsigned short int*)(line_udp_packet_ptr + 4))));

	return ((unsigned short int)((sum >> 16) + (sum & 0x0000ffff)) == 0xffff);
}

// *************************************** EthernetHeaderType *********************************************

bool EthHeader::ReadFromBuffer(const unsigned char* buffer_ptr)
{
	// check if buffer_ptr points to somewhere (hopefully) to 14 byte ethernet header
	if (NULL == buffer_ptr)
		return false;

	// read whole bunch
	*this = *((const EthHeader*)buffer_ptr);

	// correct ether type to host byte order
	eth_type = ntohs(eth_type);

	return true;
}

bool EthHeader::WriteToBuffer(unsigned char* buffer_ptr)
{
	// check if buffer_ptr points to somewhere (hopefully) to 14 byte ethernet header
	if (NULL == buffer_ptr)
		return false;

	EthHeader dummy_eth_header = *this;
	// correct ether type to network byte order
	dummy_eth_header.eth_type = htons(eth_type);

	// write whole bunch
	*((EthHeader*)buffer_ptr) = dummy_eth_header;

	return true;
}

void EthHeader::Display() const
{
	std::cout << "------------------ Ethernet Header ---------------------" << std::endl;
	std::cout << std::hex;
	std::cout << "Source mac         : " << (unsigned int)src_mac[0] << ":" << (unsigned int)src_mac[1] << ":" << (unsigned int)src_mac[2] << ":" << (unsigned int)src_mac[3] << ":" << (unsigned int)src_mac[4] << ":" << (unsigned int)src_mac[5] << std::endl;
	std::cout << "Destination max    : " << (unsigned int)dst_mac[0] << ":" << (unsigned int)dst_mac[1] << ":" << (unsigned int)dst_mac[2] << ":" << (unsigned int)dst_mac[3] << ":" << (unsigned int)dst_mac[4] << ":" << (unsigned int)dst_mac[5] << std::endl;
	std::cout << "Protocol           : " << eth_type << std::endl;
	std::cout << std::dec;
}


}