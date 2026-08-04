#include <conio.h>
#include <cmath>
#include<Windows.h>
#include"TextEditor.h"
#include "BSAI25064_String.h"
using namespace std;
// ============================================================================
// WINDOWS CONSOLE UTILITIES
// ============================================================================
void getRowColbyLeftClick(int& rpos, int& cpos) {
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD Events;
    INPUT_RECORD InputRecord;
    SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
    do {
        ReadConsoleInput(hInput, &InputRecord, 1, &Events);
        if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
            cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
            rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
            break;
        }
    } while (true);
}
void gotoRowCol(int rpos, int cpos) {
    COORD scrn;
    HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
    scrn.X = cpos;
    scrn.Y = rpos;
    SetConsoleCursorPosition(hOuput, scrn);
}
void color(int k) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, k);
}
void hideConsoleCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void SetConsoleSettings(int fontIndex) {
    // Force the entire command prompt background canvas to be 100% white instantly
    system("color F0");

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;

    // CRITICAL FIX: Explicitly zero-initialize the memory block.
    // This stops Windows from silently rejecting the font modification.
    CONSOLE_FONT_INFOEX cfi;
    ZeroMemory(&cfi, sizeof(cfi));
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;

    // Configure standard legible row text heights
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 22;

    // CRITICAL FIX: Tell Windows to look outside standard raster font blocks
    // and activate scalable TrueType vector layout assets instead.
    cfi.FontFamily = FF_DONTCARE | TMPF_TRUETYPE;
    cfi.FontWeight = FW_NORMAL;

    // --- Verified 20-Font Monospace & TrueType Map System ---
    switch (fontIndex) {
    case 1:  wcscpy_s(cfi.FaceName, L"Consolas"); break;            // Clean Coding Monospace
    case 2:  wcscpy_s(cfi.FaceName, L"Courier New"); break;         // Distinct Serif Typewriter Style
    case 3:  wcscpy_s(cfi.FaceName, L"Lucida Console"); break;      // Traditional Wide Digital Layout
    case 4:  wcscpy_s(cfi.FaceName, L"Comic Sans MS"); break;       // Handwriting / Casual Script Style
    case 5:  wcscpy_s(cfi.FaceName, L"Segoe UI"); break;            // Ultra Clean Minimalist Interface Thin
    case 6:  wcscpy_s(cfi.FaceName, L"MS Gothic"); break;           // Heavy Density Block Type Layout
    case 7:  wcscpy_s(cfi.FaceName, L"SimSun"); break;              // Delicate Compact Technical Layout
    case 8:  wcscpy_s(cfi.FaceName, L"Arial"); break;               // Standard Modern Corporate Workhorse
    case 9:  wcscpy_s(cfi.FaceName, L"Times New Roman"); break;     // Classic Formal Academic Book Look
    case 10: wcscpy_s(cfi.FaceName, L"Georgia"); break;             // Wide Artistic Curved Editorial Font
    case 11: wcscpy_s(cfi.FaceName, L"Verdana"); break;             // Modern Wide High-Contrast Digital
    case 12: wcscpy_s(cfi.FaceName, L"Trebuchet MS"); break;        // Geometric Rounded Modern Creative
    case 13: wcscpy_s(cfi.FaceName, L"Impact"); break;              // Thick Heavy Aggressive Column Block
    case 14: wcscpy_s(cfi.FaceName, L"Franklin Gothic Medium"); break; // Bold Vintage Structural News Title
    case 15: wcscpy_s(cfi.FaceName, L"Century Gothic"); break;      // Perfect Round Geometric Circle Script
    case 16: wcscpy_s(cfi.FaceName, L"Calibri"); break;             // Soft Modern Rounded Interface Text
    case 17: wcscpy_s(cfi.FaceName, L"Cambria"); break;             // Crisp Technical Modern Math Serif
    case 18: wcscpy_s(cfi.FaceName, L"Palatino Linotype"); break;   // Highly Stylized Script Calligraphy Accent
    case 19: wcscpy_s(cfi.FaceName, L"Garamond"); break;            // Elegant Ultra-Thin Classic Script Font
    case 20: wcscpy_s(cfi.FaceName, L"Terminal"); break;            // Raw Hardcore Bitmapped Programming Grid
    default: wcscpy_s(cfi.FaceName, L"Consolas"); break;
    }

    // Pass configuration variables to the Windows backend window renderer
    SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

    // Lock down visual dimensions
    COORD bufferSize = { 90, 32 };
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    SMALL_RECT windowSize = { 0, 0, 89, 31 };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}



int main() {
    TextEditor editor;
    int ch;
    int currentMode = 0; 
    bool exitApp = false;
    editor.showLandingPage();
    SetConsoleSettings(1);
    int selectedFont = editor.promptFontSelection();
    SetConsoleSettings(selectedFont);
    system("cls");
    color(240);
    editor.render(false, currentMode);
    while (!exitApp) {
        ch = _getch();
        if (currentMode == 0) {
            if (ch == 'q' or ch == 'Q') { 
                break;
            }
            if (ch == 'i' or ch == 'I') {
                currentMode = 1;
                editor.render(true, currentMode);
                continue;
            }
            if (ch == ':') { 
                currentMode = 2;
                editor.insertChar(-99);
                editor.render(false, currentMode);
                continue;
            }
            if (ch == 0 or ch == 224) {
                _getch(); 
                continue;
            }
            MyString cmd(static_cast<char>(ch));
            editor.handleNormalModeInput(cmd);
            editor.render(false, currentMode);
        }
        else if (currentMode == 1) 
        {
            if (ch == 27) { 
                currentMode = 0;
                editor.render(false, currentMode);
                continue;
            }

            if (ch == 9) {
                bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) and 0x8000) != 0;
                if (shiftPressed) {
                    editor.insertChar(25); 
                }
                else {
                    editor.insertChar(9); 
                }
                editor.render(true, currentMode);
                continue;
            }
            if (ch == 0 or ch == 224) {
                int extCh = _getch();
                bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) and 0x8000) != 0;
                if (shiftPressed) {
                    editor.insertChar(extCh * 1000);
                }
                else if (extCh == 83) {
                    editor.insertChar(83000);
                }
                else {
                    if (extCh == 75) {
                        editor.insertChar(751); // Normal Left
                    }
                    if (extCh == 77) {
                        editor.insertChar(771); // Normal Right
                    }
                    if (extCh == 72) {
                        editor.insertChar(721); // Normal Up
                    }
                    if (extCh == 80) {
                        editor.insertChar(801); // Normal Down
                    }
                }
            }
            else {
                editor.insertChar(ch);
            }
            editor.render(true, currentMode);
        }
        else if (currentMode == 2) {
            if (ch == 27 or ch == 3) { 
                currentMode = 0;
                editor.insertChar(-99);
                editor.render(false, currentMode);
                continue;
            }
            if (ch == 13) { 
                editor.executeCommandLine(exitApp);
                currentMode = 0;
                editor.render(false, currentMode);
                continue;
            }
            if (ch == 8) 
            { 
                editor.insertChar(-88); 
                editor.render(false, currentMode);
                continue;
            }
            if (ch >= 32 and ch <= 126) {
                editor.insertChar(ch + 50000);
            }
            editor.render(false, currentMode);
        }
    }
    color(7);
    system("cls");

    return 0;
}
