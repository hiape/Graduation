#pragma once
#include "configure.hpp"

namespace voip {


	//µÿ÷∑Ω·ππ
	struct IpPort
	{
		uint m_ipv4;
		unsigned short int m_port;
		IpPort(uint ipv4 = 0,unsigned short int port = 0):m_ipv4(ipv4),m_port(port){}
	};
}