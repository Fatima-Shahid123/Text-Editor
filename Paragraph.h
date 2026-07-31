#pragma once
#include"Line.h"
#include<iostream>
using namespace std;
class Paragraph {
private:
	Line** lines;
	int linecount;
	int capacity;
	void resize() {
		capacity = (capacity == 0) ? 2 : capacity * 2;
		Line** temp = new Line * [capacity];
		for (int i = 0; i < linecount; i++) {
			temp[i] = lines[i];
		}
		delete[] lines;
		lines = temp;
	}
public:
	Paragraph() :lines(nullptr), linecount(0), capacity(0) {}
	~Paragraph() {
		for (int i = 0; i < linecount; i++) {
			delete lines[i];
		}
		delete[]lines;
	}
	void addLine(const Line& newLine) {
		if (linecount >= capacity) {
			resize();
		}
		lines[linecount++] = new Line(newLine);
	}
	Line** getLines() {
		return lines;
	}
	int getLineCount()const {
		return linecount;
	}
};
