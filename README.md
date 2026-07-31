# 💻 Custom C++ Vim-Style Terminal Text Editor

A highly performant, custom-built terminal text editor developed from scratch in C++. This editor runs entirely within the Windows Console subsystem and implements a lightweight, pure architecture mimicking classical **Vim** modal patterns—featuring distinct **NORMAL** and **INSERT** operating states without depending on standard library strings or high-level wrappers.

---

## 🔥 Key Technical Highlights

* **Zero-STL Base Infrastructure:** Powered entirely by a custom-written string layout controller (`MyString`) featuring deep character mapping array controls, split/tokenize managers, and memory guards.
* **Complex Data Hierarchy Design:** Content storage utilizes an integrated multi-tiered architectural hierarchy from the ground up: `Document` ➔ `Chapters` ➔ `Sections` ➔ `Paragraph` ➔ `Line` objects.
* **Pointer Protection Layout:** Engineered memory-safe, safe tracking stacks that circumvent standard Rule of Three pointer replication crashes by executing in-place element streaming and content data snapshots.

---

## ⚡ Feature Matrix & Modal Mapping

### 🟢 1. NORMAL Mode (Navigation & Text Controls)
* **Classical Vim Movements:** Complete spatial navigation mapping layout using `h` (left), `l` (right), `j` (down), and `k` (up).
* **Granular Traversal Controls:** Jumps directly to line margins with `0` (start) and `$` (end), or file terminals via `gg` (top) and `G` (bottom).
* **Word and Page Surfing:** Real-time semantic traversal using `w` (next word start) and `b` (previous word start) across multi-line wrap margins alongside `Ctrl+d` / `Ctrl+u` half-page acceleration jumps.
* **Editing Shortcuts:** In-place modifications including `x` (character removal), `D` (truncate line tail), `~` (toggle case inversion), and the recursive `.` token to instantly re-execute your last modification command.
* **Advanced Undo & Redo Matrix:** Infinite localized time-travel history tracking states using a pure snapshot dynamic array stack (`u` for Undo and `Ctrl+r` for Redo).
* **Yank & Paste Clipboard Buffer:** Deeply copies line strings into an isolated runtime clipboard node using `yy` to paste after (`p`) or before (`P`) active document locations.

### 🟠 2. INSERT Mode (Notepad-Style Editing)
* **Real-time Document Injection:** Standard writing layouts supporting full ASCII character boundaries with built-in page margin tracking limits.
* **Text Wrapping Operations:** Context-aware `Enter` tracking that dynamically handles line splitting and wraps trailing characters downwards onto clean lines.
* **Smart Tab Mechanics:** Tab layouts indent lines forward by 4 clean spaces, with structural fallback unindent triggers via Shift+Tab.
* **Precision Erasers:** Features dual-boundary erase mechanisms via `Backspace` (collapses characters backward and pulls up prior line boundaries) and `Delete` (pulls up content from succeeding lines).
* **Dynamic Content Selection Grid:** Supports block highlighting controls, text cutting (`Ctrl+X`), text copying (`Ctrl+C`), and insertion streaming injections (`Ctrl+V`).

---

## 🛠️ Technology Stack & Requirements

* **Language Standard:** ISO C++14 / C++17
* **Environment Core APIs:** Native Win32 Console Subsystem Infrastructure (`<windows.h>`, `<conio.h>`)
* **Target OS Platform:** Microsoft Windows 10 / 11

---

## 🚀 How To Run & Build

1. Clone this repository to your local system:
   ```bash
   git clone https://github.com
   ```
2. Load the project structure into any standard C++ Integrated Development Environment (IDE) like **Microsoft Visual Studio**.
3. Compile the build targets and launch the executable artifact to spin up the clean Windows Terminal application window interface.
