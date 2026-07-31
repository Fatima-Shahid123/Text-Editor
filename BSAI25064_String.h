#pragma once
#include<iostream>
#include<fstream>
using namespace std;
class MyString{
private:
	int size;
	char* Cs;
public:
	MyString();
	MyString(char c);
	MyString(const char* p);
	MyString(int n, char c);
	MyString(int num);
	~MyString();
	void insert_at_end(char c);
	MyString(ifstream& rdr, const char* filename);
	MyString(const MyString& other);
	void print()const;
	int length()const;
	char CharacterAt(int i)const;
	void ReplaceAt(int i, char c);
	void replaceFirst(char c);
	void Shallowcopy(const MyString& other);
	void deepCopy(const  MyString& other);
	MyString& operator=(const MyString& other);
	int stoi();
	MyString itos(int num);
	MyString Trim()const;
	void clear();
	MyString ToUpper();
	MyString ToLower();
	MyString Reverse()const;
	bool isEqual(const MyString& other)const;
	bool isLess(const MyString& other)const;
	bool isGreater(const MyString& other)const;
	MyString* split(char delimeter, int& count);
	bool isDelimiter(char ch, const char* delim);
	MyString* tokenize(const char* delimeter, int& count);
};
