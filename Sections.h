#pragma once
#include"Paragraph.h"
class Sections {
private:
	MyString sectionTitle;
	Paragraph** paragraphs;
	int paracount;
	int capacity;
	void resize() {
		capacity = (capacity == 0) ? 2 : capacity * 2;
		Paragraph** temp = new Paragraph * [capacity];
		for (int i = 0; i < paracount; i++) {
			temp[i] = paragraphs[i];
		}
		delete[] paragraphs;
		paragraphs = temp;
	}
public:
    Sections() : sectionTitle(),paragraphs(nullptr),paracount(0),capacity(0) {}
    Sections(const MyString& title) : sectionTitle(title),paragraphs(nullptr),paracount(0),capacity(0) {}
    ~Sections() {
        for (int i = 0; i < paracount; i++) {
            delete paragraphs[i];
        }
        delete[] paragraphs;
    }
    void addParagraph(Paragraph* newPara) {
        if (paracount >= capacity) {
            resize();
        }
        paragraphs[paracount++] = newPara;
    }

    Paragraph** getParagraphs() {
        return paragraphs; 
    }
    int getParaCount() const { 
        return paracount;
    }
    MyString getTitle() const {
        return sectionTitle;
    }
};