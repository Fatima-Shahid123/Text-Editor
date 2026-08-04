#include "TextEditor.h"
#include "Document.h"
#include <iostream>
#include"Paragraph.h"
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <cmath>
using namespace std;
const int MAX_Line_Limit = 70;
//forward declaration
void gotoRowCol(int rpos, int cpos);
void color(int k);
/// helping functions of the private of that class
void TextEditor::splitLineAtCursor() {
	Line** lines = activePara.getLines();
	MyString currentText = lines[cr]->getText();
	//splitting the line into two halves and 
    // inserting the second half into the next line
	MyString firstHalf;
	MyString secondHalf;
	int len = currentText.length();
	for (int i = 0; i < cc; i++) {
		firstHalf.insert_at_end(currentText.CharacterAt(i));
	}
	for (int i = cc; i < len; i++) {
		secondHalf.insert_at_end(currentText.CharacterAt(i));
	}
	lines[cr]->setText(firstHalf);
	activePara.addLine(Line(""));
	lines = activePara.getLines();
	int maxLines = activePara.getLineCount();
	for (int i = maxLines - 1; i > cr + 1; i--) {
		lines[i]->setText(lines[i - 1]->getText());
	}
	lines[cr + 1]->setText(secondHalf);
	cr++;
	cc = 0;
}
void TextEditor::mergeLineWithNext() {
	int maxLines = activePara.getLineCount();
	if (cr >= maxLines - 1) {
		return;
	}
	Line** lines = activePara.getLines();
	MyString currentText = lines[cr]->getText();
	MyString nextText = lines[cr + 1]->getText();
	int nextLen = nextText.length();
	for (int i = 0; i < nextLen; i++) {
		currentText.insert_at_end(nextText.CharacterAt(i));
	}
	lines[cr]->setText(currentText);
	for (int i = cr + 1; i < maxLines - 1; i++) {
		lines[i]->setText(lines[i + 1]->getText());
	}
	lines[maxLines - 1]->setText(MyString(""));
}
void  TextEditor::clearSearchResults() {
	delete[] searchResults;
	searchResults = nullptr;
	matchCount = 0;
	currentMatchIdx = -1;
}
////overall class functions
//constructor making everything to zero and undo redo capacity=4 and making doc object in composition
TextEditor::TextEditor() : cr(0), cc(0), yankedLine(nullptr), hasYankedData(false),
undoCount(0), undoCapacity(4), redoCount(0), redoCapacity(4),
isSelecting(false), selStartRow(0), selStartCol(0) {
	//composing the document object in the text editor class
	doc = new Document("Workspace_Project");
	activePara.addLine(Line(""));
	undoStack = new ParagraphSnapshot[undoCapacity];
	redoStack = new ParagraphSnapshot[redoCapacity];
}
void TextEditor::clearRedoStack() {
    if (redoStack != nullptr) {
        for (int i = 0; i < redoCount; i++) {
            if (redoStack[i].lineTexts != nullptr) {
                delete[] redoStack[i].lineTexts;
                redoStack[i].lineTexts = nullptr;
            }
            redoStack[i].lineCount = 0;
        }
    }
    redoCount = 0;
}
//undo and redo function(helping functions)
void TextEditor::pushUndo() {
    clearRedoStack();
    if (undoCount >= undoCapacity) {
        undoCapacity = (undoCapacity == 0) ? 4 : undoCapacity * 2;
        ParagraphSnapshot* temp = new ParagraphSnapshot[undoCapacity];
        for (int i = 0; i < undoCount; i++) {
            temp[i] = undoStack[i];
        }
        delete[] undoStack;
        undoStack = temp;
    }
    int totalLines = activePara.getLineCount();
    undoStack[undoCount].lineCount = totalLines;
    undoStack[undoCount].lineTexts = new MyString[totalLines];
    Line** currentLines = activePara.getLines();
    for (int i = 0; i < totalLines; i++) {
        if (currentLines and currentLines[i]) {
            undoStack[undoCount].lineTexts[i] = currentLines[i]->getText();
        }
    }
    undoCount++;
}
//redo helping function
void TextEditor::pushRedo() {
    if (redoCount >= redoCapacity) {
        redoCapacity = (redoCapacity == 0) ? 4 : redoCapacity * 2;
        ParagraphSnapshot* temp = new ParagraphSnapshot[redoCapacity];
        for (int i = 0; i < redoCount; i++) {
            temp[i] = redoStack[i];
        }
        delete[] redoStack;
        redoStack = temp;
    }
    int totalLines = activePara.getLineCount();
    redoStack[redoCount].lineCount = totalLines;
    redoStack[redoCount].lineTexts = new MyString[totalLines];
    Line** currentLines = activePara.getLines();
    for (int i = 0; i < totalLines; i++) {
        if (currentLines and currentLines[i]) {
            redoStack[redoCount].lineTexts[i] = currentLines[i]->getText();
        }
    }
    redoCount++;
}
//destrctor deleting the composed object
TextEditor::~TextEditor() {
    delete doc;
}
//insert mode
void TextEditor::insertChar(int ch) {
    if (ch == -99) {
		// Escape key pressed, exit insert mode
        currentCommandStr.clear();
        return;
    }
    if (ch == -88) { //backspace
		// Remove the last character from currentCommandStr
        if (currentCommandStr.length() > 0) {
            MyString modified;
            for (int i = 0; i < currentCommandStr.length() - 1; i++) {
                modified.insert_at_end(currentCommandStr.CharacterAt(i));
            }
            currentCommandStr = modified;
        }
        return;
    }
    if (ch >= 50032 and ch <= 50126) { 
		// Handle special characters (arrow keys, function keys)
        char originalChar = static_cast<char>(ch - 50000);
        currentCommandStr.insert_at_end(originalChar);
        return;
    }
    int maxLines = activePara.getLineCount();
    if (maxLines == 0) {
        return;
    }
    Line** lines = activePara.getLines();
    int currentLineLen = (cr < maxLines) ? lines[cr]->getText().length() : 0;
    switch (ch) {
    case 13:
        // Enter
        pushUndo();
       {
			//Enter key pressed, split the current line at the cursor position
            MyString currentText = lines[cr]->getText();
            MyString firsthalf;
            MyString secondhalf;
            for (int i = 0; i < cc; i++) {
                firsthalf.insert_at_end(currentText.CharacterAt(i));
            }
            for (int i = cc; i < currentLineLen; i++) {
                secondhalf.insert_at_end(currentText.CharacterAt(i));
            }
            lines[cr]->setText(firsthalf);
            activePara.addLine(Line(""));
            lines = activePara.getLines();
            maxLines = activePara.getLineCount();
            for (int i = maxLines - 1; i > cr + 1; i--) {
                lines[i]->setText(lines[i - 1]->getText());
            }
            lines[cr + 1]->setText(secondhalf);
            cr++;
            cc = 0;
        }
        isSelecting = false;
        break;

    case 9:
        pushUndo();
        {
			// Tab
            MyString currentText = lines[cr]->getText();
            MyString modified;
            for (int i = 0; i < cc; i++) {
                modified.insert_at_end(currentText.CharacterAt(i));
            }
            for (int s = 0; s < 4; s++) {
                modified.insert_at_end(' ');
            }
            for (int i = cc; i < currentLineLen; i++) {
                modified.insert_at_end(currentText.CharacterAt(i));
            }
            lines[cr]->setText(modified);
            cc += 4;
        }
        isSelecting = false;
        break;

    case 25:
    {
		// Shift + Tab
        MyString currentText = lines[cr]->getText();
        int spaceCount = 0;
        // Count the number of leading spaces (up to 4)
        while (spaceCount < 4 and spaceCount < currentText.length() and currentText.CharacterAt(spaceCount) == ' ') {
            spaceCount++;
        }
        if (spaceCount > 0) {
            pushUndo();
            MyString modified;
            for (int i = spaceCount; i < currentText.length(); i++) {
                modified.insert_at_end(currentText.CharacterAt(i));
            }
            lines[cr]->setText(modified);
            cc = (cc >= spaceCount) ? cc - spaceCount : 0;
        }
    }
    isSelecting = false;
    break;
    case 22:
        if (selectedText.length() > 0) {
            pushUndo();
            MyString txt = lines[cr]->getText();
            MyString modified;
            for (int i = 0; i < cc; i++) {
                modified.insert_at_end(txt.CharacterAt(i));
            }
            for (int i = 0; i < selectedText.length(); i++) {
                modified.insert_at_end(selectedText.CharacterAt(i));
            }
            for (int i = cc; i < txt.length(); i++) {
                modified.insert_at_end(txt.CharacterAt(i));
            }
            lines[cr]->setText(modified);
            cc += selectedText.length();
            isSelecting = false;
        }
        break;

    case 24:
        if (isSelecting and cr == selStartRow) {
            pushUndo();
            selectedText.clear();
            MyString txt = lines[cr]->getText();
            MyString modified;
            int startIdx = (cc < selStartCol) ? cc : selStartCol;
            int endIdx = (cc > selStartCol) ? cc : selStartCol;
            for (int i = 0; i < txt.length(); i++) {
                if (i >= startIdx and i < endIdx) {
                    selectedText.insert_at_end(txt.CharacterAt(i));
                }
                else
                {
                    modified.insert_at_end(txt.CharacterAt(i));
                }
            }
            lines[cr]->setText(modified);
            cc = startIdx;
            isSelecting = false;
        }
        break;
    case 8:
        if (cc > 0) {
            pushUndo();
            MyString currentText = lines[cr]->getText();
            MyString modified;
            for (int i = 0; i < currentLineLen; i++) {
                if (i != cc - 1) {
                    modified.insert_at_end(currentText.CharacterAt(i));
                }
            }
            lines[cr]->setText(modified);
            cc--;
        }
        else if (cr > 0) {
            pushUndo();
            cr--;
            cc = lines[cr]->getText().length();
            MyString currentText = lines[cr]->getText();
            MyString nextText = lines[cr + 1]->getText();
            for (int i = 0; i < nextText.length(); i++) {
                currentText.insert_at_end(nextText.CharacterAt(i));
            }
            lines[cr]->setText(currentText);
            for (int i = cr + 1; i < maxLines - 1; i++) {
                lines[i]->setText(lines[i + 1]->getText());
            }
            lines[maxLines - 1]->setText(MyString(""));
        }
        isSelecting = false;
        break;

    case 3:
        if (isSelecting and cr == selStartRow) {
            selectedText.clear();
            MyString txt = lines[cr]->getText();
            int startIndex = (cc < selStartCol) ? cc : selStartCol;
            int endIndex = (cc > selStartCol) ? cc : selStartCol;
            for (int i = startIndex; i < endIndex; i++) {
                selectedText.insert_at_end(txt.CharacterAt(i));
            }
        }
        break;
    case 83000://delete key because either way we are getting the letter s printed
        if (cc < currentLineLen) {
            pushUndo();
            MyString currentText = lines[cr]->getText();
            MyString modified;
            for (int i = 0; i < currentLineLen; i++) {
                if (i != cc) {
                    modified.insert_at_end(currentText.CharacterAt(i));
                }
            }
            lines[cr]->setText(modified);
        }
        else if (cr < maxLines - 1) { 
            pushUndo();
            MyString currentText = lines[cr]->getText();
            MyString nextText = lines[cr + 1]->getText();
            for (int i = 0; i < nextText.length(); i++) {
                currentText.insert_at_end(nextText.CharacterAt(i));
            }
            lines[cr]->setText(currentText);
            for (int i = cr + 1; i < maxLines - 1; i++) {
                lines[i]->setText(lines[i + 1]->getText());
            }
            lines[maxLines - 1]->setText(MyString(""));
        }
        isSelecting = false;
        break;
        ///shift arrow keys
    case 75000: // Shift + Left Arrow
        if (!isSelecting) {
            isSelecting = true;
            selStartRow = cr;
            selStartCol = cc;
        }
        if (cc > 0) {
            cc--;
        }
        break;
    case 77000: // Shift + Right Arrow
        if (!isSelecting) {
            isSelecting = true;
            selStartRow = cr;
            selStartCol = cc;
        }
        if (cc < currentLineLen) {
            cc++;
        }
        break;
    case 72000: // Shift + Up Arrow
        if (!isSelecting) {
            isSelecting = true; 
            selStartRow = cr;
            selStartCol = cc;
        }
        if (cr > 0) {
            cr--;
            int len = lines[cr]->getText().length();
            if (cc > len) {
                cc = len;
            }
        }
        break;

    case 80000: // Shift + Down Arrow
        if (!isSelecting) {
            isSelecting = true; 
            selStartRow = cr; 
            selStartCol = cc;
        }
        if (cr < maxLines - 1) {
            cr++;
            int len = lines[cr]->getText().length();
            if (cc > len) {
                cc = len;
            }
        }
        break;
        //normal movements of the keys 
    case 751: // Normal Left Arrow
        if (cc > 0) {
            cc--;
        }
        isSelecting = false; // Normal arrows break current selection state
        break;
    case 771: // Normal Right Arrow
        if (cc < currentLineLen) {
            cc++;
        }
        isSelecting = false;
        break;
    case 721: // Normal Up Arrow
        if (cr > 0) {
            cr--;
            int len = lines[cr]->getText().length();
            if (cc > len) {
                cc = len;
            }
        }
        isSelecting = false;
        break;

    case 801: // Normal Down Arrow
        if (cr < maxLines - 1) {
            cr++;
            int len = lines[cr]->getText().length();
            if (cc > len) {
                cc = len;
            }
        }
        isSelecting = false;
        break;
    default:
        if (ch >= 32 and ch <= 126) {
            if (cc >= MAX_Line_Limit) {
                activePara.addLine(Line(""));
                cr++;
                cc = 0;
                lines = activePara.getLines();
            }
            MyString currentText = lines[cr]->getText();
            MyString modified;
            for (int i = 0; i < cc; i++) {
                modified.insert_at_end(currentText.CharacterAt(i));
            }
            modified.insert_at_end(static_cast<char>(ch));
            for (int i = cc; i < currentText.length(); i++) {
                modified.insert_at_end(currentText.CharacterAt(i));
            }
            lines[cr]->setText(modified);
            cc++;
            isSelecting = false;
        }
        break;
    }
}
void TextEditor::handleNormalModeInput(const MyString& command) {
    int maxLines = activePara.getLineCount();
    Line** lines = activePara.getLines();
    int currentLineLen = (cr < maxLines) ? lines[cr]->getText().length() : 0;
    char keyChar = command.CharacterAt(0);
    if (lastPrefix == 'g' and keyChar == 'g') {
        cr = 0;
        cc = 0;
        lastPrefix = '\0';
        return;
    }
    if (lastPrefix == 'd' and keyChar=='d'){
        pushUndo();
        if (maxLines > 1) {
            Paragraph temp;
            for (int i = 0; i < maxLines; i++) {
                if (i != cr) {
                    temp.addLine(*(lines[i]));
                }
            }
            activePara = temp;
            maxLines = activePara.getLineCount();
            lines = activePara.getLines(); 
            if (cr >= maxLines) {
                cr = maxLines - 1;
            }
            cc = 0;
        }
        else {
            lines[0]->setText(MyString(""));
            cc = 0;
        }
        lastCommand = MyString("dd");
        lastPrefix = '\0';
        return;
    }
    if (lastPrefix == 'y' and keyChar == 'y'){
        delete yankedLine;
        yankedLine = new Line(*(lines[cr]));
        hasYankedData = true;
        lastPrefix = '\0';
        return;
    }
    if (keyChar == 'g' or keyChar == 'd' or keyChar == 'y') {
        lastPrefix = keyChar;
        return;
    }
    lastPrefix = '\0';
    switch (keyChar) {

    case 'h':
        cc = (cc > 0) ? cc - 1 : 0;
        break;
    case 'l':
        if (cc < currentLineLen) {
            cc++;
        }
        break;

    case 'j':
        if (cr < maxLines - 1) {
            cr++;
            int newLineLen = lines[cr]->getText().length();
            if (cc > newLineLen) {
                cc = newLineLen;
            }
        }
        break;

    case 'k':             
        if (cr > 0) {
            cr--;
            int newLineLen = lines[cr]->getText().length();
            if (cc > newLineLen) {
                cc = newLineLen;
            }
        }
        break;
    case '0':
        cc = 0;
        break;
    case '$':
        cc = currentLineLen;
        break;
    case 'G':
        cr = maxLines - 1;
        cc = lines[cr]->getText().length();
        break;
    case 4: 
        cr = (cr + 10 < maxLines) ? cr + 10 : maxLines - 1;
        {
            int newLineLen = lines[cr]->getText().length();
            if (cc > newLineLen) {
                cc = newLineLen;
            }
        }
        break;
    case 21: 
        cr = (cr - 10 > 0) ? cr - 10 : 0;
        {
            int newLineLen = lines[cr]->getText().length();
            if (cc > newLineLen) {
                cc = newLineLen;
            }
        }
        break;
    case '.':
        if (lastCommand.length() > 0) {
            MyString tempCmd = lastCommand;
            lastCommand = MyString("");
            handleNormalModeInput(tempCmd);
            lastCommand = tempCmd;
        }
        break;
    case '~':
        if (currentLineLen > 0 and cc < currentLineLen) {
            pushUndo();
            char targetChar = lines[cr]->getText().CharacterAt(cc);
            if (targetChar >= 'a' and targetChar <= 'z') {
                targetChar -= 32;
            }
            else if (targetChar >= 'A' and targetChar <= 'Z') {
                targetChar += 32;
            }
            lines[cr]->getText().ReplaceAt(cc, targetChar);
            lastCommand = MyString("~");
        }
        break;
    case 'x':
        if (currentLineLen > 0 and cc < currentLineLen) {
            pushUndo();
            MyString original = lines[cr]->getText();
            MyString modified;
            for (int i = 0; i < currentLineLen; i++) {
                if (i != cc) {
                    modified.insert_at_end(original.CharacterAt(i));
                }
            }
            lines[cr]->setText(modified);
            if (cc >= modified.length() and cc > 0) {
                cc = modified.length() - 1;
            }
            lastCommand = MyString("x");
        }
        break;
    case 'D':
        pushUndo();
        {
            MyString original = lines[cr]->getText();
            MyString modified;
            for (int i = 0; i < cc; i++) {
                modified.insert_at_end(original.CharacterAt(i));
            }
            lines[cr]->setText(modified);
            if (cc > 0) {
                cc--;
            }
            lastCommand = MyString("D");
        }
        break;
    case 'p':
        if (hasYankedData and yankedLine){
            pushUndo();
            Paragraph temp;
            for (int i = 0; i <= cr; i++) {
                temp.addLine(*(lines[i]));
            }
            temp.addLine(*yankedLine);
            for (int i = cr + 1; i < maxLines; i++) {
                temp.addLine(*(lines[i]));
            }
            activePara = temp;
            maxLines = activePara.getLineCount();
            lines = activePara.getLines(); 
            cr++; 
            cc = 0;
            lastCommand = MyString("p");
        }
        break;
    case 'P': 
        if (hasYankedData and yankedLine) {
            pushUndo();
            Paragraph temp;
            for (int i = 0; i < cr; i++) {
                temp.addLine(*(lines[i]));
            }
            temp.addLine(*yankedLine);
            for (int i = cr; i < maxLines; i++) {
                temp.addLine(*(lines[i]));
            }
            activePara = temp;
            maxLines = activePara.getLineCount();
            lines = activePara.getLines(); 
            cc = 0;
            lastCommand = MyString("P");
        }
        break;
    case 'u': 
        if (undoCount > 0) {
            pushRedo();
            undoCount--;
            int snapCount = undoStack[undoCount].lineCount;
            MyString* snapTexts = undoStack[undoCount].lineTexts;
            if (snapTexts != nullptr and snapCount > 0) {
                Paragraph temp;
                for (int i = 0; i < snapCount; i++) {
                    Line structuralLine;
                    structuralLine.setText(snapTexts[i]);
                    temp.addLine(structuralLine);
                }
                activePara = temp;
                delete[] snapTexts;
                undoStack[undoCount].lineTexts = nullptr;
                undoStack[undoCount].lineCount = 0;
            }
            maxLines = activePara.getLineCount();
            lines = activePara.getLines();
            if (cr >= maxLines) {
                cr = (maxLines > 0) ? maxLines - 1 : 0;
            }
            if (cr < 0) {
                cr = 0;
            }
            cc = 0;
            return; 
        }
        break;

    case 18: 
        if (redoCount > 0) {
            pushUndo();
            redoCount--;
            int snapCount = redoStack[redoCount].lineCount;
            MyString* snapTexts = redoStack[redoCount].lineTexts;
            if (snapTexts != nullptr and snapCount > 0) {
                Paragraph temp;
                for (int i = 0; i < snapCount; i++) {
                    Line structuralLine;
                    structuralLine.setText(snapTexts[i]);
                    temp.addLine(structuralLine);
                }
                activePara = temp;
                delete[] snapTexts;
                redoStack[redoCount].lineTexts = nullptr;
                redoStack[redoCount].lineCount = 0;
            }
            maxLines = activePara.getLineCount();
            lines = activePara.getLines();
            if (cr >= maxLines) {
                cr = (maxLines > 0) ? maxLines - 1 : 0;
            }
            if (cr < 0) {
                cr = 0;
            }
            cc = 0;
            return; 
        }
        break;

    case 'w':
    {
        
        MyString txt = lines[cr]->getText();
        int idx = cc;
        while (idx < currentLineLen and txt.CharacterAt(idx) != ' ' and txt.CharacterAt(idx) != '\t') {
            idx++;
        }
        while (idx < currentLineLen and (txt.CharacterAt(idx) == ' ' or txt.CharacterAt(idx) == '\t')) {
            idx++;
        }
        if (idx < currentLineLen) {
            cc = idx;
        }
        else if (cr < maxLines - 1) {
            cr++;
            cc = 0;
            MyString nexttxt = lines[cr]->getText();
            int nextLen = nexttxt.length();
            while (cc < nextLen and (nexttxt.CharacterAt(cc) == ' ' or nexttxt.CharacterAt(cc) == '\t')) {
                cc++;
            }
        }
        break;
    }
    case 'b':
    {
        MyString txt = lines[cr]->getText();
        int idx = cc - 1;
        while (idx >= 0 and (txt.CharacterAt(idx) == '\t' or txt.CharacterAt(idx) == ' ')) {
            idx--;
        }
        while (idx > 0 and txt.CharacterAt(idx - 1) != ' ' and txt.CharacterAt(idx - 1) != '\t') {
            idx--;
        }
        if (idx >= 0) {
            cc = idx;
        }
        else if (cr > 0) {
            cr--;
            MyString prevTxt = lines[cr]->getText();
            int preLen = prevTxt.length();
            idx = preLen - 1;
            while (idx >= 0 and (prevTxt.CharacterAt(idx) == ' ' or prevTxt.CharacterAt(idx) == '\t')) {
                idx--;
            }
            while (idx > 0 and prevTxt.CharacterAt(idx - 1) != ' ' and prevTxt.CharacterAt(idx - 1) != '\t') {
                idx--;
            }
            cc = (idx >= 0) ? idx : 0;
        }
        break;

    }
    case 'n': 
        if (matchCount > 0) {
            if (lastSearchDirection)
            {
                currentMatchIdx = (currentMatchIdx + 1) % matchCount;
            }
            else
            { 
                currentMatchIdx = (currentMatchIdx - 1 + matchCount) % matchCount;
            }
            cr = searchResults[currentMatchIdx].row;
            cc = searchResults[currentMatchIdx].col;
        }
        break;
    case 'N': 
        if (matchCount > 0)
        {
            if (lastSearchDirection) { 
                currentMatchIdx = (currentMatchIdx - 1 + matchCount) % matchCount;
            }
            else
            { 
                currentMatchIdx = (currentMatchIdx + 1) % matchCount;
            }
            cr = searchResults[currentMatchIdx].row;
            cc = searchResults[currentMatchIdx].col;
        }
        break;
    default:
        break;

    }

}
void TextEditor::executeCommandLine(bool& exitApp) {
    if (currentCommandStr.length() == 0) {
        return;
    }
    int maxLines = activePara.getLineCount();
    if (maxLines == 0) {
        return;
    }
    Line** lines = activePara.getLines();
    if (currentCommandStr.isEqual(MyString("q!"))) {
        exitApp = true;
        return;
    }
    if (currentCommandStr.isEqual(MyString("q"))) {
        if (isModified) {
            currentCommandStr = MyString("Error: Unsaved changes! Use :q! to override.");
            return;
        }
        exitApp = true;
        return;
    }
    if (currentCommandStr.isEqual(MyString("w")) or currentCommandStr.isEqual(MyString("wq"))) {
        ofstream out("Project.txt");
        if (out.is_open()) {
            for (int i = 0; i < maxLines; i++) {
                MyString txt = lines[i]->getText();
                for (int j = 0; j < txt.length(); j++) {
                    out.put(txt.CharacterAt(j));
                }
                out.put('\n');
            }
            out.close();
            isModified = false;

            if (currentCommandStr.isEqual(MyString("wq"))) {
                exitApp = true;
                return;
            }
            currentCommandStr = MyString("Document saved");
            return;
        }
        else
        {
            currentCommandStr = MyString("Error");
            return;
        }
    }
    if (currentCommandStr.isEqual(MyString("set number"))) {
        showLineNumbers = true;
        currentCommandStr.clear();
        return;
    }
    if (currentCommandStr.isEqual(MyString("set nonumber"))) {
        showLineNumbers = false;
        currentCommandStr.clear();
        return;
    }
    char prefix = currentCommandStr.CharacterAt(0);
    if (prefix == '/' or prefix == '?') {
        clearSearchResults();
        lastSearchDirection = (prefix == '/');
        MyString pattern;
        for (int i = 1; i < currentCommandStr.length(); i++) {
            pattern.insert_at_end(currentCommandStr.CharacterAt(i));
        }
        lastSearchPattern = pattern;
        if (pattern.length() > 0) {
            int tempCapacity = 10;
            searchResults = new SearchMatch[tempCapacity];
            for (int r = 0; r < maxLines; r++) {
                MyString src = lines[r]->getText();
                int srcLen = src.length();
                int patLen = pattern.length();
                for (int c = 0; c <= srcLen - patLen; c++) {
                    bool matchFound = true;
                    for (int k = 0; k < patLen; k++) {
                        if (src.CharacterAt(c + k) != pattern.CharacterAt(k)) {
                            matchFound = false;
                            break;
                        }
                    }
                    if (matchFound) {
                        if (matchCount >= tempCapacity) {
                            tempCapacity *= 2;
                            SearchMatch* tempArr = new SearchMatch[tempCapacity];
                            for (int i = 0; i < matchCount; i++) {
                                tempArr[i] = searchResults[i];
                            }
                            delete[] searchResults;
                            searchResults = tempArr;
                        }
                        searchResults[matchCount].row = r;
                        searchResults[matchCount].col = c;
                        matchCount++;
                    }
                }
            }

            if (matchCount > 0) {
                if (lastSearchDirection) {
                    currentMatchIdx = 0;
                }
                else {
                    currentMatchIdx = matchCount - 1;
                    for (int i = matchCount - 1; i >= 0; i--) {
                        if (searchResults[i].row < cr or (searchResults[i].row == cr and searchResults[i].col < cc)) {
                            currentMatchIdx = i;
                            break;
                        }
                    }
                }
                cr = searchResults[currentMatchIdx].row;
                cc = searchResults[currentMatchIdx].col;
                currentCommandStr = MyString("Matches discovered layout: ") + MyString(matchCount);
            }
            else
            {
                currentCommandStr = MyString("Pattern not found in context.");
            }
        }
        return;
    }

    if (currentCommandStr.length() > 3 and currentCommandStr.CharacterAt(0) == '%' and currentCommandStr.CharacterAt(1) == 's' and currentCommandStr.CharacterAt(2) == '/') {
        int firstSlash = 2;
        int secondSlash = -1;
        int thirdSlash = -1;
        for (int i = 3; i < currentCommandStr.length(); i++) {
            if (currentCommandStr.CharacterAt(i) == '/') {
                if (secondSlash == -1) secondSlash = i;
                else if (thirdSlash == -1) thirdSlash = i;
            }
        }

        if (secondSlash != -1 and thirdSlash != -1 and currentCommandStr.CharacterAt(thirdSlash + 1) == 'g') {
            pushUndo();
            MyString oldWord, newWord;
            for (int i = firstSlash + 1; i < secondSlash; i++) {
                oldWord.insert_at_end(currentCommandStr.CharacterAt(i));
            }
            for (int i = secondSlash + 1; i < thirdSlash; i++) {
                newWord.insert_at_end(currentCommandStr.CharacterAt(i));
            }
            int replaceCount = 0;
            for (int r = 0; r < maxLines; r++) {
                MyString original = lines[r]->getText();
                MyString transformed;
                int oLen = original.length();
                int wLen = oldWord.length();
                for (int c = 0; c < oLen; ) {
                    bool match = false;
                    if (c <= oLen - wLen) {
                        match = true;
                        for (int k = 0; k < wLen; k++) {
                            if (original.CharacterAt(c + k) != oldWord.CharacterAt(k)) {
                                match = false;
                                break;
                            }
                        }
                    }
                    if (match) {
                        for (int k = 0; k < newWord.length(); k++) {
                            transformed.insert_at_end(newWord.CharacterAt(k));
                        }
                        c += wLen;
                        replaceCount++;
                    }
                    else
                    {
                        transformed.insert_at_end(original.CharacterAt(c));
                        c++;
                    }
                }
                lines[r]->setText(transformed);
            }
            currentCommandStr = MyString("Substituted phrases safely: ") + MyString(replaceCount);
            return;
        }
    }
}
void TextEditor::showLandingPage() {
    system("color F0");
    system("cls");
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { 85, 28 };
    SetConsoleScreenBufferSize(hOut, bufferSize);
    cout << "\n";
    color(245); cout << "   _____________________"; color(240); cout << "                                       "; color(241); cout << "  _____________________ \n";
    color(245); cout << "  |  _________________  |"; color(240); cout << "                                     "; color(241); cout << "  |  _________________  |\n";
    color(245); cout << "  | |  "; color(244); cout << "STICKY NOTE:   "; color(245); cout << "| |";
    color(240); cout << "                                     ";
    color(241); cout << "  | |  "; color(244); cout << "STICKY NOTE:   "; color(241); cout << "| |\n";
    color(245); cout << "  | |  - Write daily  | |"; color(240); cout << "                                     "; color(241); cout << "  | |  - Edit clean   | |\n";
    color(245); cout << "  | |  - Code fast    | |"; color(240); cout << "                                     "; color(241); cout << "  | |  - Build OOP    | |\n";
    color(245); cout << "  | |_________________| |"; color(240); cout << "                                     "; color(241); cout << "  | |_________________| |\n";
    color(245); cout << "  |_____________________|"; color(240); cout << "                                     "; color(241); cout << "  |_____________________|\n";
    cout << "                                                                                     \n";
    cout << "                                                                                     \n";
    color(244); cout << "                        ===========================================                  \n";
    color(240); cout << "                              T E X T   E D I T O R                                  \n";
    cout << "                                                                                     \n";
    cout << "                                    B Y   "; color(241); cout << "F A T I M A                                \n";
    color(244); cout << "                        ===========================================                  \n";
    cout << "                                                                                     \n";
    cout << "                                                                                     \n";
    color(242); cout << "                                Press [ ENTER ] to Launch                            \n";
    color(240); cout << "                                                                                     \n";
    cout << "                                                                                     \n";
    color(242); cout << "    ___________   ___________"; color(240); cout << "                               "; color(245); cout << "___________   ___________\n";
    color(242); cout << "   /          /_ /          /"; color(240); cout << "                              "; color(245); cout << " /          /_ /          /\n";
    color(242); cout << "  /  OPEN    /  /   BOOK   /"; color(240); cout << "                              "; color(245); cout << "/  FATIMA  /  /  ENGINE  /\n";
    color(242); cout << " /  NOTE_   /  /   ____   /"; color(240); cout << "                              "; color(245); cout << "/  _CODE_  /  /   ____   /\n";
    color(242); cout << "/__________/  /__________/"; color(240); cout << "                              "; color(245); cout << "/__________/  /__________/ \n";

    cout << "\n";
    color(240);
    while (true) {
        char ch = _getch();
        if (ch == 13) {
            break;
        }
    }
    system("cls");
}
int TextEditor::promptFontSelection() {
    system("cls");
    color(240); 
    cout << "\n";
    cout << "   =======================================================================   \n";
    cout << "                     CHOOSE YOUR NOTEBOOK FONT TYPOGRAPHY                  \n";
    cout << "   =======================================================================   \n\n";
    color(245);
    cout << "    01. Consolas (Coding Mono)        "; color(241); cout << " 11. Verdana (High-Contrast Wide)\n"; color(245);
    cout << "    02. Courier New (Typewriter)      "; color(241); cout << " 12. Trebuchet MS (Rounded Modern)\n"; color(245);
    cout << "    03. Lucida Console (Digital Wide) "; color(241); cout << " 13. Impact (Thick Block Letters)\n"; color(245);
    cout << "    04. Comic Sans MS (Handwriting)   "; color(241); cout << " 14. Franklin Gothic (News Type)\n"; color(245);
    cout << "    05. Segoe UI (Minimalist Thin)    "; color(241); cout << " 15. Century Gothic (Geometric)\n"; color(245);
    cout << "    06. MS Gothic (Heavy Density)     "; color(241); cout << " 16. Calibri (Soft Curved Clean)\n"; color(245);
    cout << "    07. SimSun (Delicate Technical)   "; color(241); cout << " 17. Cambria (Math Technical)\n"; color(245);
    cout << "    08. Arial (Modern Corporate)      "; color(241); cout << " 18. Palatino (Calligraphy Tilt)\n"; color(245);
    cout << "    09. Times New Roman (Book Serif)  "; color(241); cout << " 19. Garamond (Ultra-Thin Classic)\n"; color(245);
    cout << "    10. Georgia (Curved Editorial)    "; color(241); cout << " 20. Terminal (MS-DOS Hardcore Mono)\n\n";

    color(244); // Crimson line separator
    cout << "   -----------------------------------------------------------------------   \n";
    color(240); cout << "    Type your font index number (01 to 20): ";

    MyString typedNumber;
    while (true) {
        char ch = _getch();

        if (ch >= '0' and ch <= '9') {
            if (typedNumber.length() < 2) {
                typedNumber.insert_at_end(ch);
                cout << ch;
            }
        }
        else if (ch == 8 and typedNumber.length() > 0) {
            MyString popped;
            for (int i = 0; i < typedNumber.length() - 1; i++) {
                popped.insert_at_end(typedNumber.CharacterAt(i));
            }
            typedNumber = popped;
            cout << "\b \b";
        }
        else if (ch == 13 and typedNumber.length() > 0) {
            int parsedIndex = typedNumber.stoi();
            if (parsedIndex >= 1 and parsedIndex <= 20) {
                return parsedIndex;
            }
            else {
                cout << "\r                                                                      \r";
                color(244); cout << "    [Invalid Number! Try 01-20]: "; color(240);
                typedNumber.clear();
            }
        }
    }
}




void TextEditor::render(bool insertMode, int inputState) {
    char title[150];
    sprintf_s(title, "Text Editor | Mode: %s | Pos: (%d, %d) %s",
        (inputState == 1) ? "INSERT" : (inputState == 2) ? "COMMAND" : "NORMAL",
        cr, cc, isSelecting ? "| SELECTING" : "");
    SetConsoleTitleA(title);
    gotoRowCol(0, 0);
    color(240);
    Line** lines = activePara.getLines();
    int totalLines = activePara.getLineCount();
    for (int i = 0; i < totalLines; i++) {
        color(240);
        for (int s = 0; s < MAX_Line_Limit + 15; s++) {
            cout << " ";
        }
        cout << "\r"; 
        if (showLineNumbers) {
            color(248); 
            if (i + 1 < 10) cout << " ";
            cout << i + 1 << " | ";
            color(240); 
        }
        MyString currentText = lines[i]->getText();
        int lineLen = currentText.length();
        for (int j = 0; j < lineLen; j++) {
            bool highlightChar = false;
            if (isSelecting) {
                int startRow = (selStartRow < cr) ? selStartRow : cr;
                int endRow = (selStartRow > cr) ? selStartRow : cr;
                if (startRow != endRow) {
                    if (i == startRow) {
                        int anchorCol = (startRow == selStartRow) ? selStartCol : cc;
                        if (j >= anchorCol) {
                            highlightChar = true;
                        }
                    }
                    else if (i == endRow) {
                        int terminalCol = (endRow == selStartRow) ? selStartCol : cc;
                        if (j < terminalCol) {
                            highlightChar = true;
                        }
                    }
                    else if (i > startRow and i < endRow) {
                        highlightChar = true;
                    }
                }
                else if (i == startRow) {
                    int leftCol = (selStartCol < cc) ? selStartCol : cc;
                    int rightCol = (selStartCol > cc) ? selStartCol : cc;
                    if (j >= leftCol and j < rightCol) {
                        highlightChar = true;
                    }
                }
            }

            if (highlightChar) {
                color(15);
                cout << currentText.CharacterAt(j);
                color(240);  
            }
            else {
                cout << currentText.CharacterAt(j);
            }
        }
        cout << "\n";
    }
    gotoRowCol(totalLines + 1, 0);
    color(240);
    for (int s = 0; s < MAX_Line_Limit + 15; s++) {
        cout << " ";
    }
    cout << "\r"; 
    if (inputState == 2) {
        color(244); 
        cout << ":";
        currentCommandStr.print();
        color(240);
    }
    else if (currentCommandStr.length() > 0) {
        color(242);
        currentCommandStr.print();
        color(240);
    }
    if (inputState == 2) {
        gotoRowCol(totalLines + 1, currentCommandStr.length() + 1);
    }
    else {
        int targetedCol = cc + (showLineNumbers ? 5 : 0);
        gotoRowCol(cr, targetedCol);
    }
}

