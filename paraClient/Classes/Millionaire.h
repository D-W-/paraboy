#pragma once
#include "BigNum.h"
#include<vector>
#include "RSA.cpp"
#include <cmath>
/*
Bob使用Alice的公钥加密数据,发送给Alice
publicKey:  n  e
*/
vector<BigNum> millionSendMessage(int level, pair<BigNum, BigNum>publicKey);

vector<BigNum> millionDealMessage(int level, pair<BigNum, BigNum> privateKey, BigNum receiveMessage);


bool millionRich(int level, BigNum& x, vector<BigNum>reply);

double random(double start, double end);






