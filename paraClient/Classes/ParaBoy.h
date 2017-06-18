#pragma once
#ifndef __PARABOY_H__
#define __PARABOY_H__

#include "cocos2d.h"
#include "string"
#include <RSA.cpp>

// 128位的十进制位数, 一个int表示4个digit
#define RSA_KEY_LENGTH 20

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
	int level;
	BigNum* x;
public:
	ParaBoy();
	void setId(string id);
	string getID();
	void initRSA();
	//String getSSID();
	static ParaBoy* create(const std::string& normalImage, const std::string& selectedImage, const ccMenuCallback& callback);
	string getPrivateKey();
	void getPublicKey(string& d, string& n);
	void setPrivateKey(string e);
	void setPublicKey(string d, string n);

	string getCipher();
	string verifyCipher(string cipher);
	int getLevel();
	void setLevel(int level);
	BigNum* getX();
	void setX(BigNum& x);
	void setCode(int x, int y);
	int getCodeX();
	int getCodeY();
};

#endif // !__PAPABOY_H__
