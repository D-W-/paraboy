#pragma once
#include "BigNum.h"
#include<vector>
#include "RSA.cpp"
#include <cmath>
/*
Bobʹ��Alice�Ĺ�Կ��������,���͸�Alice
publicKey:  n  e
*/
vector<BigNum> millionSendMessage(int level, pair<BigNum, BigNum>publicKey);

vector<BigNum> millionDealMessage(int level, pair<BigNum, BigNum> privateKey, BigNum receiveMessage);


bool millionRich(int level, BigNum& x, vector<BigNum>reply);

double random(double start, double end);






