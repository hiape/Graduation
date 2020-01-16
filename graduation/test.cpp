#include "configure.hpp"
#include "consumer.hpp"
#include "ipport.hpp"
#include "rowsocket.hpp"
#include <iostream>

using namespace std;
using namespace voip;

int main()
{

	PcapConsumer* p = new PcapConsumer();
	if(!p)
		cout<<"create error"<<endl;
	else 
		cout<<"create success"<<endl;


	
	delete p;

	return 0;
}