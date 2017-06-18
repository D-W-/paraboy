#include <RSA.cpp>

#include <string>
#include <unordered_map>

// 128位的十进制位数, 一个int表示4个digit
#define RSA_KEY_LENGTH 10

class ParaUtil {
private:
	RSA* rsa;
	string d;
	string n;

public:
	ParaUtil() {
		rsa = new RSA(RSA_KEY_LENGTH);
		d = "";
		n = "";
	}

	void generateKeys() {
		rsa->generateKeys();
		this->d = rsa->getD();
		this->n = rsa->getN();
	}

	string encrypt(string msg) {
		if (!d.empty()) {
			return rsa->encrypt(msg);
		}
		return "";
	}

	string decrypt(string msg, string d, string n) {
		return rsa->decrypt(msg, d, n);
	}

};


