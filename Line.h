#pragma once
#include<iostream>
#include"BSAI25064_String.h"
class Line
{
	MyString text;
public:
	Line() :text() {}
	Line(const char* str) :text(str) {}
	Line(const MyString& str) :text(str) {}
	MyString& getText() {
		return text;
	}
	const MyString& getText()const {
		return text;
	}
	void setText(const MyString& str) {
		text = str;
	}
};

