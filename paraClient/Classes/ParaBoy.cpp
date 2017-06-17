#include "ParaBoy.h"

ParaBoy::ParaBoy()
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

void ParaBoy::getPrivateKey(string & e, string & n)
{
	e = this->privateE;
	n = this->publicN;
}

void ParaBoy::getPublicKey(string & d, string & n)
{
	d = this->publicD;
	n = this->publicN;
}

void ParaBoy::setPrivateKey(string e, string n)
{
	this->privateE = e;
	this->publicN = n;
}

void ParaBoy::setPublicKey(string d, string n)
{
	this->publicD = d;
	this->publicN = n;
}
