#include "ParaBoy.h"

ParaBoy::ParaBoy():
	id(""),
	privateE(""),
	publicD(""),
	publicN(""),
	level(1),
	codeX(0),
	codeY(0)
{

}

void ParaBoy::setId(string id)
{
	this->id = id;
}

string ParaBoy::getID()
{
	return id;
}

void ParaBoy::initRSA()
{
	rsa = new RSA(RSA_KEY_LENGTH);
	rsa->generateKeys();
	this->publicD = rsa->getD();
	this->privateE = rsa->getE();
	this->publicN = rsa->getN();
}

ParaBoy * ParaBoy::create(const std::string & normalImage, const std::string & selectedImage, const ccMenuCallback & callback)
{
	ParaBoy *ret = new (std::nothrow) ParaBoy();
	if (ret && ret->initWithNormalImage(normalImage, selectedImage, "", callback))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

string ParaBoy::getPrivateKey()
{
	return this->privateE;
}

void ParaBoy::getPublicKey(string & d, string & n)
{
	d = this->publicD;
	n = this->publicN;
}

void ParaBoy::setPrivateKey(string e)
{
	this->privateE = e;

}

void ParaBoy::setPublicKey(string d, string n)
{
	this->publicD = d;
	this->publicN = n;
}

string ParaBoy::getCipher()
{
	string result = "";
	if (rsa == NULL) 
		rsa = new RSA(RSA_KEY_LENGTH);
	if (privateE != "")
		result = rsa->encrypt("helloworld", privateE, publicN);
	return result;
}

string ParaBoy::verifyCipher(string cipher)
{
	string result = "";
	if (rsa == NULL)
		rsa = new RSA(RSA_KEY_LENGTH);
	if (publicD != "")
		result = rsa->decrypt(cipher, publicD, publicN);
	return result;
}

int ParaBoy::getLevel()
{
	return this->level;
}

void ParaBoy::setLevel(int level)
{
	this->level = level;
}

BigNum* ParaBoy::getX()
{
	return this->x;
}

void ParaBoy::setX(BigNum & x)
{
	this->x = new BigNum(x);
}

void ParaBoy::setCode(int x, int y)
{
	this->codeX = x;
	this->codeY = y;
}

int ParaBoy::getCodeX()
{
	return this->codeX;
}

int ParaBoy::getCodeY()
{
	return this->codeY;
}
