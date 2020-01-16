#pragma once
#include "configure.h"

#include <string>

namespace voip{

	//加密类
	class Encryption{
		
	public:
		//构造函数
		Encryption(){}

		virtual	~Encryption() {}

		//数据加密方法
		virtual bool DataEncryption(char* data_ptr, unsigned int data_size,char** new_data_ptr,unsigned int& new_data_size) = 0;

	};


	//  不对数据进行加密
	class NoEncryption :public Encryption{

	public:
		NoEncryption() {}

		virtual ~NoEncryption() {}

		//数据加密方法
		virtual bool DataEncryption(char* data_ptr, unsigned int data_size,char** new_data_ptr,unsigned int& new_data_size)
		{
			//不用加密，直接返回成功
			return true;
		}

	};

	//  AES加密方式
	class AesEncryption :public Encryption{

	public:
		AesEncryption() {}

		virtual ~AesEncryption() {}

		//数据加密方法
		virtual bool DataEncryption(char* data_ptr, unsigned int data_size,char** new_data_ptr,unsigned int& new_data_size)
		{
			return true;
		}

	};


	//加密工厂
	class EncryptionFactory
	{
	public:
		
		EncryptionFactory() {}
		
		virtual ~EncryptionFactory() {}
		
		virtual Encryption* CreateEncryption() const = 0;

		virtual const std::string GetEncryptionName() const = 0;
	};


	//没有加密方法的加密工厂
	class NoEncryptionFactory : public EncryptionFactory
	{
	public:
	
		NoEncryptionFactory() { }

		virtual ~NoEncryptionFactory() {}

		virtual Encryption* CreateEncryption() const  { return new NoEncryption();}

		virtual const std::string GetEncryptionName() const { return std::string("None");}
	};

	//AES方法的加密工厂
	class AesEncryptionFactory : public EncryptionFactory
	{
	private:

	public:
	
		AesEncryptionFactory() { }

		virtual ~AesEncryptionFactory() {}

		virtual Encryption* CreateEncryption() const  { return new AesEncryption();}

		virtual const std::string GetEncryptionName() const { return std::string("AES");}
	};


}