// ============================================================
//  sudoku.cpp  —  SudokuBoard Class Implementation
//  DSA Sudoku Solver  |  MSc IT Minor Project
// ============================================================
#include "sudoku.h"
#include "utils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>   // for memset

// ─────────────────────────────────────────────────────────────
//  CONSTRUCTORS
// ─────────────────────────────────────────────────────────────

// Default constructor: initialise all cells to 0 (empty)
SudokuBoard::SudokuBoard() {
    // memset is a fast way to zero-out a fixed-size array.
    // It sets every byte to 0.
    memset(grid,     0, sizeof(grid));
    memset(original, 0, sizeof(original));
}

// Copy constructor: deep-copy from another board
// Useful when we want to save a board state before solving.
SudokuBoard::SudokuBoard(const SudokuBoard& other) {
    memcpy(grid,     other.grid,     sizeof(grid));
    memcpy(original, other.original, sizeof(original));
}

// ─────────────────────────────────────────────────────────────
//  LOADING PUZZLES
// ─────────────────────────────────────────────────────────────

// Load from an 81-character string (compact format)
// Example: "530070000600195000098000060..."
bool SudokuBoard::loadFromString(const std::string& s) {
    if (s.length() < 81) {
        std::cerr << Color::RED << "[Error] Puzzle string must be 81 chars.\n" << Color::RESET;
        return false;
    }
    clear();
    for (int i = 0; i < 81; i++) {
        int row = i / GRID_SIZE;   // Integer division gives row
        int col = i % GRID_SIZE;   // Remainder gives column
        int val = s[i] - '0';      // Convert char digit to int
        grid[row][col] = val;
        original[row][col] = (val != EMPTY_CELL); // Mark non-zero as original clue
    }
    return true;
}

// Load from a .txt file
// The file format: 9 lines, each with 9 digits (0 = empty)
// Lines starting with '#' are comments and are skipped.
bool SudokuBoard::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << Color::RED << "[Error] Cannot open file: " << filename << Color::RESET << "\n";
        return false;
    }

    clear();
    int row = 0;
    std::string line;

    while (std::getline(file, line) && row < GRID_SIZE) {
        if (line.empty() || line[0] == '#') continue; // Skip comments

        int col = 0;
        for (char ch : line) {
            if (col >= GRID_SIZE) break;
            if (ch >= '0' && ch <= '9') {
                int val = ch - '0';
                grid[row][col] = val;
                original[row][col] = (val != EMPTY_CELL);
                col++;
            }
        }
        if (col > 0) row++; // Only advance row if we read at least one digit
    }

    file.close();

    if (row < GRID_SIZE) {
        std::cerr << Color::YELLOW << "[Warning] File had only " << row << " rows.\n" << Color::RESET;
    }
    return true;
}

// Interactive input from the user
// User enters digits row by row, 0 for empty cells.
void SudokuBoard::loadFromInput() {
    clear();
    std::cout << Color::BRIGHT_CYAN
              << "\n  Enter the puzzle row by row.\n"
              << "  Use 0 for empty cells. You can use spaces.\n"
              << "  Example: 5 3 0 0 7 0 0 0 0\n\n"
              << Color::RESET;

    for (int row = 0; row < GRID_SIZE; row++) {
        std::cout << Color::YELLOW << "  Row " << (row + 1) << ": " << Color::RESET;
        int col = 0;
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        int val;
        while (iss >> val && col < GRID_SIZE) {
            if (val < 0 || val > 9) {
                std::cerr << Color::RED << "  Invalid digit. Using 0.\n" << Color::RESET;
                val = 0;
            }
            grid[row][col] = val;
            original[row][col] = (val != EMPTY_CELL);
            col++;
        }
        if (col < GRID_SIZE) {
            std::cout << Color::YELLOW << "  [Warning] Only " << col << " digits given for row " << (row+1) << ". Rest filled with 0.\n" << Color::RESET;
        }
    }
    std::cout << Color::BRIGHT_GREEN << "\n  ✓ Puzzle loaded!\n" << Color::RESET;
}

// ─────────────────────────────────────────────────────────────
//  BOARD ACCESS
// ─────────────────────────────────────────────────────────────

int  SudokuBoard::getCell(int r, int c)       const { return grid[r][c]; }
void SudokuBoard::setCell(int r, int c, int v)      { grid[r][c] = v;    }
bool SudokuBoard::isEmpty(int r, int c)        const { return grid[r][c] == EMPTY_CELL; }
bool SudokuBoard::isCellOriginal(int r, int c) const { return original[r][c]; }

// ─────────────────────────────────────────────────────────────
//  VALIDATION (CONSTRAINT CHECKING)
//  ─────────────────────────────────────────────────────────────
//  CORE DSA CONCEPT: Constraint Satisfaction Problem (CSP)
//  Sudoku rules = three constraints:
//    1. Each row must have digits 1–9 exactly once.
//    2. Each column must have digits 1–9 exactly once.
//    3. Each 3×3 box must have digits 1–9 exactly once.
// ─────────────────────────────────────────────────────────────

// Check if 'num' is already in the same row
bool SudokuBoard::isValidRow(int row, int num) const {
    for (int col = 0; col < GRID_SIZE; col++) {
        if (grid[row][col] == num) return false;
    }
    return true;
}

// Check if 'num' is already in the same column
bool SudokuBoard::isValidCol(int col, int num) const {
    for (int row = 0; row < GRID_SIZE; row++) {
        if (grid[row][col] == num) return false;
    }
    return true;
}

// Check if 'num' is already in the 3×3 box that contains (boxRow, boxCol)
// boxRow and boxCol are the TOP-LEFT corner of the box.
bool SudokuBoard::isValidBox(int boxRow, int boxCol, int num) const {
    for (int r = 0; r < BOX_SIZE; r++) {
        for (int c = 0; c < BOX_SIZE; c++) {
            if (grid[boxRow + r][boxCol + c] == num) return false;
        }
    }
    return true;
}

// Master validity check: is it safe to place 'num' at (row, col)?
// This calls all three constraint checks above.
// TIME COMPLEXITY: O(3N) = O(N) where N = GRID_SIZE = 9
// (The bitmask solver will improve this to O(1)!)
bool SudokuBoard::isValid(int row, int col, int num) const {
    // Find the top-left corner of the 3×3 box
    // Example: cell (5, 7) → box top-left = (3, 6)
    int boxRow = (row / BOX_SIZE) * BOX_SIZE;
    int boxCol = (col / BOX_SIZE) * BOX_SIZE;

    return isValidRow(row, num)      &&
           isValidCol(col, num)      &&
           isValidBox(boxRow, boxCol, num);
}

// Check if the board is fully solved (no empty cells + consistent)
bool SudokuBoard::isSolved() const {
    return countEmpty() == 0 && isConsistent();
}

// Check if the board has no constraint violations
bool SudokuBoard::isConsistent() const {
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            int val = grid[row][col];
            if (val == EMPTY_CELL) continue;
            // Temporarily clear the cell to check if placing
            // the same value back would be valid
            const_cast<SudokuBoard*>(this)->grid[row][col] = 0;
            bool ok = isValid(row, col, val);
            const_cast<SudokuBoard*>(this)->grid[row][col] = val;
            if (!ok) return false;
        }
    }
    return true;
}

// Count how many cells are empty
int SudokuBoard::countEmpty() const {
    int count = 0;
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (grid[r][c] == EMPTY_CELL) count++;
    return count;
}

// ─────────────────────────────────────────────────────────────
//  DISPLAY
// ─────────────────────────────────────────────────────────────

// Pretty-print the board with colors and box separators
//
// Visual layout:
//   ┌───────┬───────┬───────┐
//   │ 5 3 . │ . 7 . │ . . . │
//   │ 6 . . │ 1 9 5 │ . . . │
//   ...
//   └───────┴───────┴───────┘
//
// Color coding:
//   CYAN  = original clue (given by the puzzle)
//   GREEN = solver-filled digit
//   DIM   = empty cell (shown as dot)
void SudokuBoard::print() const {
    std::cout << "\n";
    std::cout << Color::DIM << "    ┌───────┬───────┬───────┐\n" << Color::RESET;

    for (int row = 0; row < GRID_SIZE; row++) {
        // Print left border
        std::cout << Color::DIM << "    │ " << Color::RESET;

        for (int col = 0; col < GRID_SIZE; col++) {
            int val = grid[row][col];

            if (val == EMPTY_CELL) {
                // Empty cell shown as a dim dot
                std::cout << Color::DIM << "." << Color::RESET;
            } else if (original[row][col]) {
                // Original clue — bright cyan and bold
                std::cout << Color::BOLD << Color::BRIGHT_CYAN << val << Color::RESET;
            } else {
                // Solver-filled cell — bright green
                std::cout << Color::BRIGHT_GREEN << val << Color::RESET;
            }

            // Print column separator at box boundaries
            if (col == 2 || col == 5) {
                std::cout << Color::DIM << " │ " << Color::RESET;
            } else if (col < 8) {
                std::cout << " ";
            }
        }

        // Print right border
        std::cout << Color::DIM << " │\n" << Color::RESET;

        // Print row separator at box boundaries
        if (row == 2 || row == 5) {
            std::cout << Color::DIM << "    ├───────┼───────┼───────┤\n" << Color::RESET;
        }
    }

    std::cout << Color::DIM << "    └───────┴───────┴───────┘\n" << Color::RESET;
    std::cout << "\n";
}

// Plain board (no ANSI codes) — useful for file output or
// terminals that don't support colors.
void SudokuBoard::printPlain() const {
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            std::cout << grid[row][col];
            if (col < 8) std::cout << " ";
        }
        std::cout << "\n";
    }
}

// ─────────────────────────────────────────────────────────────
//  UTILITY
// ─────────────────────────────────────────────────────────────

// Reset: undo all solver-placed cells, keep original clues
void SudokuBoard::reset() {
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            if (!original[r][c]) grid[r][c] = EMPTY_CELL;
}

// Clear: wipe everything including original flags
void SudokuBoard::clear() {
    memset(grid,     0, sizeof(grid));
    memset(original, 0, sizeof(original));
}
