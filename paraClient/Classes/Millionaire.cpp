#pragma once
#include "Millionaire.h"
#include <vector>
#include "Utils.cpp"

/*
Bob使用Alice的公钥加密数据,发送给Alice
Alice publicKey:  n  e
level:Bob的level
返回为一个vector
第一个元素为x,需要保留下来用于与Alice返回的消息作比较
第二个元素为k=level,需要发送给Alice
*/
const int N = 25;

vector<BigNum> millionSendMessage(int level, pair<BigNum, BigNum>publicKey) {
	RSA rsa;
	//随机生成一个N bit的数 N=30
	int x = random(1 << N, 1 <<(N+1));
	//int x = 597824102;
	stringstream ss;
	ss << x;
	BigNum k = rsa.encrypt(BigNum(ss.str()), publicKey.second, publicKey.first);
	vector<BigNum>res;
	res.push_back(BigNum(ss.str()));
	res.push_back(k - level);
	cout << "k-level:" << k-level << endl;
	return res;
}

/*
*Alice处理从Bob那里接收到的信息receiveMessage(k-level)
*level:Alice的level
*privateKey: Alice的私钥： n  d
*/
vector<BigNum> millionDealMessage(int level,pair<BigNum, BigNum> privateKey,BigNum receiveMessage) {
	vector<BigNum>yu;
	RSA rsa;
	for (int u = 1; u <= N; u++) {
		BigNum temp = rsa.decrypt(BigNum(receiveMessage + u), privateKey.second, privateKey.first);
		yu.push_back(temp);
	}
	//生成一个长度为(p/2)bit的素数
	BigNum* p = Utils::getPrime(N / 2);
	//BigNum* p =new BigNum("27803");
	vector<BigNum>zu;
	//将p值放入返回值之中
	zu.push_back(*p);

	bool flag = true;
	while (flag) {
		for (int i = 0; i < yu.size(); i++) {
			BigNum temp = yu[i] % (*p);
			if (flag && i > 0 && temp != zu[zu.size() - 1]) {
				flag = false;
			}
			zu.push_back(temp);
		}
		if (flag) {
			p = Utils::getPrime(N / 2);
			zu.clear();
			zu.push_back(*p);
		}
	}

	vector<BigNum>wu;
	wu.push_back(*p);
	for (int i = 0; i < yu.size(); i++) {
		BigNum temp = zu[i + 1];
		if ((i + 1) <= level) {
			wu.push_back(temp);
		}
		else {
			wu.push_back((temp + 1) % (*p));
		}
	}

	return wu;

}
/*
Bob根据Alice返回给自己的消息,判断自己是否比Alice富有
第一个元素是p
接下来是数组
假如第j个元素== x (mod p) 那么Alice 富有,返回true
否则Bob富有,返回False
level:Bob的军衔
*/

bool millionRich(int level,BigNum& x,vector<BigNum>reply) {
	BigNum p = reply[0];
	//cout << x % p << "   judge  " << reply[level];
	return x % p == reply[level];
}

double random(double start, double end)
{
	return start + (end - start)*rand() / (RAND_MAX + 1.0);
}
