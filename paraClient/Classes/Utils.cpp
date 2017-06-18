#pragma once
#include <ctime>
#include <cstdlib>

#include "BigNum.h"

#define RAND_NUM_BITS 4
#define RAND_NUM_SIZE 10000
#define MILLER_RABIN_TIME 5



const int ZERO = 0;
const int ONE = 1;

using namespace std;

class Utils {
public:
	//这里的bitCount是大数类型的十进制位数, 不是二进制位数
	static BigNum* generateRandomNumber(int bitCount) {
		int round = bitCount / RAND_NUM_BITS;
		string buff = "";
		for (int iter = 0; iter < round; ++iter) {
			string store = string(RAND_NUM_BITS, '0');
			string randStr = to_string(rand() % RAND_NUM_SIZE);
			store.replace(0, randStr.length(), randStr);
			buff += store;
		}
		int remain = bitCount% RAND_NUM_BITS;
		string randStr = to_string(rand() % (int(pow(10, remain))));
		buff += randStr;
		return new BigNum(buff);
	}

	static BigNum* getPrime(int bitCount) {
		BigNum* random = generateRandomNumber(bitCount);
		int temp = (*random) % 2;
		if (temp == 0)
			(*random) -= 1;
		while (!millerRabin(*random)) {
			random->operator+=(2);
		}
		return random;
	}

	static bool millerRabin(BigNum& n) {
		for (int i = 0; i < MILLER_RABIN_TIME; ++i) {
			BigNum* random = generateRandomNumber(n.length());
			random->operator%=(n);
			if (gcd(*random, n) > ONE)
				return false;
			if (!passMillerRabin(*random, n))
				return false;
		}
		return true;
	}

	static bool passMillerRabin(BigNum& a, BigNum& n) {
		BigNum nMinusOne = n - 1;
		BigNum d = nMinusOne;
		int s = 0;
		while (d % 2 == 0) {
			s++;
			d /= 2;
		}
		BigNum aToPower = modPow(a, d, n);
		if (aToPower == ONE)
			return true;
		for (int i = 0; i < s - 1; i++) {
			if (aToPower == nMinusOne)
				return true;
			aToPower = (aToPower*aToPower) % n;
		}
		if (aToPower == nMinusOne)
			return true;
		return false;
	}

	static BigNum gcd(BigNum a, BigNum b)
	{
		//return b == ZERO ? a : gcd(b, a%b);
		BigNum temp = a%b;
		while (temp != 0) {
			a = b;
			b = temp;
			temp = a%b;
		}
		return b;
	}


	static BigNum modPow(BigNum n, BigNum p, BigNum m) {
		//快速计算(n^e)%m的值
		BigNum k("1");
		n %= m;
		while (p != 1)
		{
			if (0 != (p % 2))
				k = (k*n) % m;
			n = (n*n) % m;
			p /= 2;
		}
		return(n*k) % m;
	}

	static BigNum exgcd(BigNum m, BigNum n, BigNum &x, BigNum &y) {
		//扩展的Euclid mx + ny = 1
		BigNum x1, y1, x0, y0;
		x0 = 1; y0 = 0;
		x1 = 0; y1 = 1;
		x = 0; y = 1;
		BigNum r = m%n;
		BigNum q = (m - r) / n;
		while (r != 0)
		{
			x = x0 - q*x1; y = y0 - q*y1;
			x0 = x1; y0 = y1;
			x1 = x; y1 = y;
			m = n; n = r; r = m%n;
			q = (m - r) / n;
		}
		return n;
	}

};
