#pragma once
#include "Millionaire.h"
#include <vector>
#include "Utils.cpp"

/*
Bobʹ��Alice�Ĺ�Կ��������,���͸�Alice
Alice publicKey:  n  e
level:Bob��level
����Ϊһ��vector
��һ��Ԫ��Ϊx,��Ҫ��������������Alice���ص���Ϣ���Ƚ�
�ڶ���Ԫ��Ϊk=level,��Ҫ���͸�Alice
*/
const int N = 25;

vector<BigNum> millionSendMessage(int level, pair<BigNum, BigNum>publicKey) {
	RSA rsa;
	//�������һ��N bit���� N=30
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
*Alice�����Bob������յ�����ϢreceiveMessage(k-level)
*level:Alice��level
*privateKey: Alice��˽Կ�� n  d
*/
vector<BigNum> millionDealMessage(int level,pair<BigNum, BigNum> privateKey,BigNum receiveMessage) {
	vector<BigNum>yu;
	RSA rsa;
	for (int u = 1; u <= N; u++) {
		BigNum temp = rsa.decrypt(BigNum(receiveMessage+u) ,privateKey.second,privateKey.first);
		yu.push_back(temp);
	}
	//����һ������Ϊ(p/2)bit������
	BigNum* p = Utils::getPrime(N/2);
	//BigNum* p =new BigNum("27803");
	vector<BigNum>zu;
	//��pֵ���뷵��ֵ֮��
	zu.push_back(*p);
	for (int i = 0; i < yu.size(); i++) {
		BigNum temp = yu[i] % (*p);
		if ((i + 1) <= level) {
			zu.push_back(temp);
		}
		else {
			zu.push_back((temp+1)%(*p));
		}
		cout << "i:" << i << "   " << zu[i] << endl;
	}

	return zu;
}
/*
Bob����Alice���ظ��Լ�����Ϣ,�ж��Լ��Ƿ��Alice����
��һ��Ԫ����p
������������
�����j��Ԫ��== x (mod p) ��ôAlice ����,����true
����Bob����,����False
level:Bob�ľ���
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
