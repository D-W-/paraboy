#pragma once
#ifndef __PARABOY_H__
#define __PARABOY_H__

#include "cocos2d.h"
#include "string"
#include <RSA\RSA.h>

// 128位的十进制位数, 一个int表示4个digit
#define RSA_KEY_LENGTH 10

USING_NS_CC;
using namespace std;

class ParaBoy : public MenuItemImage
{
private:
	string id;
	string publicD;
	string publicN;
	string privateE;
	int codeX;
	int codeY;
	RSA* rsa;
public:
	ParaBoy();
	void setId(string id);
	string getID();
	void initRSA();
	//String getSSID();
	static ParaBoy* create(const std::string& normalImage, const std::string& selectedImage, const ccMenuCallback& callback);
	void getPrivateKey(string& e, string& n);
	void getPublicKey(string& d, string& n);
	void setPrivateKey(string e, string n);
	void setPublicKey(string d, string n);
};

#endif // !__PAPABOY_H__
