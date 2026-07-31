#pragma once
#include"Chapters.h"
#include<iostream>
using namespace std;
class Document {
private:
    MyString docName;
    Chapters** chapters;
    int chapterCount;
    int capacity;
    void resize() {
        capacity = (capacity == 0) ? 2 : capacity * 2;
        Chapters** temp = new Chapters * [capacity];
        for (int i = 0; i < chapterCount; i++) {
            temp[i] = chapters[i];
        }
        delete[] chapters;
        chapters = temp;
    }

public:
    Document() : docName("Untitled"), chapters(nullptr), chapterCount(0), capacity(0) {}
    Document(const MyString& name) : docName(name), chapters(nullptr), chapterCount(0), capacity(0) {}
    ~Document() {
        for (int i = 0; i < chapterCount; i++) {
            delete chapters[i];
        }
        delete[] chapters;
    }
    void addChapter(Chapters* newCh) {
        if (chapterCount >= capacity) {
            resize();
        }
        chapters[chapterCount++] = newCh;
    }
    Chapters** getChapters() { 
        return chapters;
    }
    int getChapterCount() const { 
        return chapterCount;
    }
};
