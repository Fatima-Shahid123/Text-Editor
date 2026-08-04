#pragma once
class Document;
class Paragraph;
#include<iostream>
using namespace std;
#include"Paragraph.h"
struct ParagraphSnapshot {
	MyString* lineTexts;
	int lineCount;
	ParagraphSnapshot() {
		lineTexts = nullptr;
		lineCount = 0;
	}
	~ParagraphSnapshot() {
		if (lineTexts != nullptr) {
			delete[]lineTexts;
			lineTexts = nullptr;
		}
	}
	ParagraphSnapshot(const ParagraphSnapshot& other) {
		lineCount = other.lineCount;
		if (other.lineTexts != nullptr) {
			lineTexts = new MyString[lineCount];
			for (int i = 0; i < lineCount; i++) {
				lineTexts[i] = other.lineTexts[i]; 
			}
		}
		else {
			lineTexts = nullptr;
		}
	}
	ParagraphSnapshot& operator=(const ParagraphSnapshot& other){
		if (lineTexts != nullptr) {
			delete[] lineTexts;
		}
		lineCount = other.lineCount;
		if (other.lineTexts != nullptr) {
			lineTexts = new MyString[lineCount];
			for (int i = 0; i < lineCount; i++) {
				lineTexts[i] = other.lineTexts[i];
			}
		}
		else 
		{
			lineTexts = nullptr;
		}
		return *this;
	}
};
class TextEditor
{
private:
	Document* doc;
	int cr;
	int cc;
	Paragraph activePara;
	MyString lastCommand;
	char lastPrefix;
	Line* yankedLine;
	bool hasYankedData;
	ParagraphSnapshot* undoStack;
	int undoCount;
	int undoCapacity;
	ParagraphSnapshot* redoStack;
	int redoCount;
	int redoCapacity;
	void clearRedoStack();
	void pushUndo();
	void pushRedo();
	bool isSelecting;
	int selStartRow;
	int selStartCol;
	MyString selectedText;
	void splitLineAtCursor();
	void mergeLineWithNext();
	MyString currentCommandStr;
	bool showLineNumbers;       
	bool isModified;            
	MyString lastSearchPattern; 
	bool lastSearchDirection;   
	struct SearchMatch {
		int row;
		int col;
	};
	SearchMatch* searchResults; 
	int matchCount;            
	int currentMatchIdx;      
	void clearSearchResults();
public:
	TextEditor();
	~TextEditor();
	void insertChar(int ch);
	void handleNormalModeInput(const MyString& command);
	void render(bool insertMode, int inputState);
	void executeCommandLine(bool& exitApp);
	void showLandingPage();
	int promptFontSelection();
};
