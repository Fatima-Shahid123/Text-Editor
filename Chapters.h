#pragma once
#pragma once
#include"Sections.h"
class Chapters {
private:
    MyString  ChapterTitle;
    Sections** sections;
    int sectioncount;
    int capacity;
    void resize() {
        capacity = (capacity == 0) ? 2 : capacity * 2;
        Sections** temp = new Sections * [capacity];
        for (int i = 0; i < sectioncount; i++) {
            temp[i] = sections[i];
        }
        delete[] sections;
        sections = temp;
    }
public:
   Chapters() : ChapterTitle(),sections(nullptr), sectioncount(0), capacity(0) {}
   Chapters(const MyString& title) :ChapterTitle(title),sections(nullptr), sectioncount(0), capacity(0) {}
    ~Chapters() {
        for (int i = 0; i < sectioncount; i++) {
            delete sections[i];
        }
        delete[]sections;
    }
    void addChapters(Sections* newPara) {
        if (sectioncount >= capacity) {
            resize();
        }
        sections[sectioncount++] = newPara;
    }

   Sections** getsections() {
        return sections;
    }
    int getSectionCount() const {
        return sectioncount;
    }
    MyString getTitle() const {
        return ChapterTitle;
    }
};