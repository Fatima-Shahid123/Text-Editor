#include "BSAI25064_String.h"
#include <iostream>
using namespace std;
MyString::MyString() {
	size = 0;
	Cs = new char[1];
	Cs[0] = '\0';
}
MyString::MyString(char c) {
	size = 1;
	Cs = new char[2];
	Cs[0] = c;
	Cs[1] = '\0';
}
MyString::MyString(const char* p) {
	size = 0;
	if (p) {
		while (p[size]!='\0') {
			size++;
		}
	}
	Cs = new char[size + 1];
	for (int i = 0; i < size; i++) {
		Cs[i] = p[i];
	}
	Cs[size] = '\0';
}
MyString::MyString(int n, char c) {
	size = (n < 0) ? 0 : n;
	Cs = new char[size + 1];
	for (int i = 0; i < size; i++) {
		Cs[i] = c;
	}
	Cs[size] = '\0';
}
MyString::MyString(int num) {
	if (num == 0) {
		size = 1;
		Cs = new char[2];
		Cs[0] = '0';
		Cs[1] = '\0';
		return;
	}
	bool negative = false;
	long long n = num; //prevent over flow
	if (n < 0) {
		negative = true;
		n = -n;
	}
	long long temp = n;
	int digits = 0;
	while (temp > 0) {
		digits++;
		temp /= 10;
	}
	size = digits + (negative ? 1 : 0);
	Cs = new char[size + 1];

	int index = size - 1;
	while (n > 0) {
		Cs[index--] = (n % 10) + '0';
		n /= 10;
	}
	if (negative) {
		Cs[0] = '-';
	}
	Cs[size] = '\0';
}
MyString::~MyString() {
	delete[] Cs;
}
void MyString::insert_at_end(char c) {
	char* temp = new char[size + 2];
	for (int i = 0; i < size; i++) {
		temp[i] = Cs[i];
	}
	temp[size] = c;
	temp[size + 1] = '\0';
	delete[] Cs;
	Cs = temp;
	size++;
}
MyString::MyString(ifstream& rdr, const char* filename) {
	size = 0;
	Cs = new char[1];
	Cs[0] = '\0';
	if (!rdr) {
		cout << "Cannot open the file" << endl;
		return;
	}
	char c;
	while (rdr.get(c)) {
		insert_at_end(c);
	}
}
MyString::MyString(const MyString& other) {
	size = other.size;
	Cs = new char[size + 1];
	for (int i = 0; i <= size; i++) {
		Cs[i] = other.Cs[i];
	}
}
void MyString::print() const {
	if (Cs) {
		cout << Cs;
	}
}
int MyString::length() const {
	return size;
}
char MyString::CharacterAt(int i) const {
	if (i >= 0 and i < size) {
		return Cs[i];
	}
	return '\0';
}
void MyString::ReplaceAt(int i, char c) {
	if (i >= 0 and i < size) {
		Cs[i] = c;
	}
}
void MyString::replaceFirst(char c) {
	if (size > 0) {
		Cs[0] = c;
	}
}
void MyString::Shallowcopy(const MyString& other) {
	size = other.size;
	Cs = other.Cs;
}
void MyString::deepCopy(const MyString& other) {
	delete[] Cs; 
	size = other.size;
	Cs = new char[size + 1];
	for (int i = 0; i <= size; i++) {
		Cs[i] = other.Cs[i];
	}
}
MyString& MyString::operator=(const MyString& other) {
	if (this != &other) {
		delete[] Cs; 
		size = other.size;
		Cs = new char[size + 1];
		for (int i = 0; i <= size; i++) {
			Cs[i] = other.Cs[i];
		}
	}
	return *this;
}
int MyString::stoi() {
	int result = 0;
	int sign = 1;
	int i = 0;
	if (size > 0 and Cs[0] == '-') {
		sign = -1;
		i++;
	}
	for (; i < size; i++) {
		if (Cs[i] >= '0' and Cs[i] <= '9') {
			result = result * 10 + (Cs[i] - '0');
		}
	}
	return result * sign;
}
MyString MyString::itos(int num) {
	return MyString(num);
}
MyString MyString::Trim() const {
	int start = 0;
	int end = size - 1;
	while (start < size and (Cs[start] == ' ' or Cs[start] == '\n' or Cs[start] == '\t')) {
		start++;
	}
	while (end >= start and (Cs[end] == ' ' or Cs[end] == '\n' or Cs[end] == '\t')) {
		end--;
	}
	MyString result;
	for (int i = start; i <= end; i++) {
		result.insert_at_end(Cs[i]);
	}
	return result;
}
void MyString::clear() {
	delete[] Cs;
	size = 0;
	Cs = new char[1];
	Cs[0] = '\0';
}

MyString MyString::ToUpper() {
	for (int i = 0; i < size; i++) {
		if (Cs[i] >= 'a' and Cs[i] <= 'z') {
			Cs[i] = Cs[i] - 32;
		}
	}
	return *this;
}
MyString MyString::ToLower() {
	for (int i = 0; i < size; i++) {
		if (Cs[i] >= 'A' and Cs[i] <= 'Z') {
			Cs[i] = Cs[i] + 32;
		}
	}
	return *this;
}
bool MyString::isEqual(const MyString& other)const {
	if (size != other.size) {
		return false;
	}
	for (int i = 0; i < size; i++) {
		if (Cs[i] != other.Cs[i]) {
			return false;
		}
	}
	return true;
}

bool MyString::isLess(const MyString& other)const {
	int min = (size < other.size) ? size : other.size;
	for (int i = 0; i < min; i++) {
		if (Cs[i] < other.Cs[i]) {
			return true;
		}
		if (Cs[i] > other.Cs[i]) {
			return false;
		}
	}
	return size < other.size;
}
bool MyString::isGreater(const MyString& other)const {
	int min = (size < other.size) ? size : other.size;
	for (int i = 0; i < min; i++) {
		if (Cs[i] > other.Cs[i]) {
			return true;
		}
		if (Cs[i] < other.Cs[i]) {
			return false;
		}
	}
	return size > other.size;
}

MyString* MyString::split(char delimeter, int& count) {
	count = 0;
	int i = 0;
	while (Cs[i] != '\0') {
		while(Cs[i] == delimeter) {
			i++;
		}
		if(Cs[i] == '\0') {
			break;
		}
		count++;
		while (Cs[i] != delimeter and Cs[i] != '\0') {
			i++;
		}
	}
	if (count == 0)
	{
		return nullptr;
	}

	MyString* arr = new MyString[count];
	i = 0;
	for (int k = 0; k < count; k++) {
		while (Cs[i] == delimeter) {
			i++;
		}
		int len = 0;
		while (Cs[i + len] != delimeter and Cs[i + len] != '\0') {
			len++;
		}
		delete[] arr[k].Cs; 
		arr[k].size = len;
		arr[k].Cs = new char[len + 1];
		for (int j = 0; j < len; j++) {
			arr[k].Cs[j] = Cs[i + j];
		}
		arr[k].Cs[len] = '\0';
		i += len;
	}
	return arr;
}

// Helper Delimiter Checker
bool MyString::isDelimiter(char ch, const char* delim) {
	if (!delim) return false;
	for (int i = 0; delim[i] != '\0'; i++) {
		if (ch == delim[i]) return true;
	}
	return false;
}

// Tokenize Utility
MyString* MyString::tokenize(const char* delimeter, int& count) {
	count = 0;
	int i = 0;
	while (Cs[i] != '\0') {
		while (isDelimiter(Cs[i], delimeter)) {
			i++;
		}
		if (Cs[i] == '\0') {
			break;
		}
		count++;
		while (Cs[i] != '\0' and !isDelimiter(Cs[i], delimeter)) {
			i++;
		}
	}
	if (count == 0) {
		return nullptr;
	}

	MyString* arr = new MyString[count];
	i = 0;
	for (int k = 0; k < count; k++) {
		while (isDelimiter(Cs[i], delimeter)) {
			i++;
		}
		int len = 0;
		while (Cs[i + len] != '\0' and !isDelimiter(Cs[i + len], delimeter)) {
			len++;
		}
		delete[] arr[k].Cs;
		arr[k].size = len;
		arr[k].Cs = new char[len + 1];
		for (int j = 0; j < len; j++) {
			arr[k].Cs[j] = Cs[i + j];
		}
		arr[k].Cs[len] = '\0';
		i += len;
	}
	return arr;
}
