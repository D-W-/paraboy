#include "Utils.cpp"

using namespace std;

#define ASCII_SIZE 128

class RSA {
private:
	int RSA_KEY_LENGTH;
	BigNum p;
	BigNum q;

	BigNum e;
	BigNum d;
	BigNum n;
	BigNum phiN;

	void generateE() {
		BigNum temp;
		while (true) {
			temp = *Utils::getPrime(RSA_KEY_LENGTH);
			if (Utils::gcd(temp, phiN) == 1)
				break;
		}
		this->e = temp;
	}

	void generateD() {
		Utils::exgcd(e, phiN, d, BigNum());
		while (d < 0) {
			d = d + phiN;
		}
	}

	BigNum _encrypt(BigNum& message) {
		return Utils::modPow(message, e, n);
	}

	BigNum _decrypt(BigNum& ciphertext) {
		return Utils::modPow(ciphertext, d, n);
	}

	BigNum _encode(string message) {
		BigNum result("0");
		for (int i = 0; i < message.length(); ++i) {
			result = (result * ASCII_SIZE) + int(message[i]);
		}
		return result;
	}

	string _decode(BigNum& message) {
		string result = "";
		while (message.length() != 0) {
			char temp = message % ASCII_SIZE;
			result.push_back(temp);
			message /= ASCII_SIZE;
		}
		reverse(result.begin(), result.end());
		return result;
	}

public:

	RSA(int keySize) : RSA_KEY_LENGTH(keySize) {
		generateKeys();
	}

	RSA() {
		RSA_KEY_LENGTH = 10;
		generateKeys();
	}

	string encrypt(string message) {
		BigNum msg = this->_encode(message);
		BigNum cipher = this->_encrypt(msg);
		return bigNumToStr(cipher);
	}

	string decrypt(string message) {
		BigNum cipher(message);
		BigNum msg = this->_decrypt(cipher);
		return this->_decode(msg);
	}

	string decrypt(string message, string _d, string _n) {
		BigNum d(_d);
		BigNum n(_n);
		BigNum cipher(message);
		BigNum msg = Utils::modPow(cipher, d, n);
		return this->_decode(msg);
	}

	string bigNumToStr(BigNum& bignum) {
		std::ostringstream oss;
		oss << bignum << endl;
		return oss.str().substr(0, oss.str().length()-1);
	}

	void generateKeys() {
		srand((unsigned)time(NULL));
		this->p = *Utils::getPrime(RSA_KEY_LENGTH);
		this->q = *Utils::getPrime(RSA_KEY_LENGTH);
		n = p*q;
		phiN = (p - 1)*(q - 1);
		generateE();
		generateD();
	}

	void setKeyLength(int keyLength) {
		this->RSA_KEY_LENGTH = keyLength;
	}

	int getKeyLength() {
		return this->RSA_KEY_LENGTH;
	}

	string getN() {
		return bigNumToStr(this->n);
	}

	string getD() {
		return bigNumToStr(this->d);
	}

	string getE() {
		return bigNumToStr(this->e);
	}

	BigNum getP() {
		return this->p;
	}

	BigNum getQ() {
		return this->q;
	}
	
	void print() {
		cout << p << endl;
		cout << q << endl;
		cout << e << endl;
		cout << d << endl;
	}
};
