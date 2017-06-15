#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>

//������һ������4λ
#define DIGIT   4
//ת��Ϊ����ʱģ10000
#define DEPTH   10000
#define MAX     200
typedef int bignum_t[MAX + 1];

using namespace std;

class BigNum
{
public:
	bignum_t* _num;
	bignum_t num;
	int sgn;
public:
	BigNum();
	BigNum(string str);
	BigNum(BigNum& other);

	int operator!();
	BigNum&operator=(const BigNum&a);
	BigNum&operator=(const int a);

	BigNum&operator+=(const BigNum&a);
	BigNum&operator+=(const int a);
	BigNum operator+(const BigNum&a);
	BigNum operator+(const int a);
	BigNum&operator-=(const BigNum&a);
	BigNum&operator-=(const int a);
	BigNum operator-(const BigNum&a);
	BigNum operator-(const int a);
	BigNum&operator*=(const BigNum&a);
	BigNum&operator*=(const int a);
	BigNum operator*(const BigNum&a);
	BigNum operator*(const int a);
	BigNum&operator/=(const BigNum&a);
	BigNum&operator/=(const int a);
	BigNum operator/(const BigNum&a);
	BigNum operator/(const int a);
	BigNum&operator%=(const BigNum&a);
	int operator%=(const int a);
	BigNum operator%(const BigNum&a);
	int operator%(const int a);
	BigNum&operator++();
	BigNum&operator--();

	int operator>(const BigNum&a);
	int operator>(const int a);
	int operator>=(const BigNum&a);
	int operator>=(const int a);
	int operator<(const BigNum&a);
	int operator<(const int a);
	int operator<=(const BigNum&a);
	int operator<=(const int a);
	int operator==(const BigNum&a);
	int operator==(const int a);
	int operator!=(const BigNum&a);
	int operator!=(const int a);
	int operator[](const int a);
	friend  istream&operator >> (istream&is, BigNum&a);
	friend  ostream&operator<<(ostream&os, const BigNum&a);

	int length();
	int zeronum();
};