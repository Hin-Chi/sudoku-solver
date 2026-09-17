// ============================================================
//  sudoku.h  —  SudokuBoard Class Declaration
//  DSA Sudoku Solver  |  MSc IT Minor Project
// ============================================================
//
//  This file defines the SudokuBoard class.
//  The "declaration" (header) tells the compiler WHAT the class
//  looks like — data members and method signatures.
//  The actual code ("definition") lives in sudoku.cpp.
//
//  WHY SPLIT .h and .cpp?
//  Separation of interface (what) from implementation (how).
//  This is a core principle of modular C++ design.
// ============================================================
#pragma once

#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────
//  CONSTANTS
//  Using constexpr means the value is resolved at compile time
//  (no runtime overhead). These describe the 9×9 grid.
// ─────────────────────────────────────────────────────────────
constexpr int GRID_SIZE  = 9;   // Total rows / columns
constexpr int BOX_SIZE   = 3;   // Size of each 3×3 sub-box
constexpr int EMPTY_CELL = 0;   // We use 0 to represent a blank


// ─────────────────────────────────────────────────────────────
//  SudokuBoard CLASS
//
//  DSA DATA STRUCTURE: 2D Array (Matrix)
//  The puzzle is stored as a 9×9 integer array:
//      board[row][col] = digit (0 = empty, 1–9 = filled)
//
//  KNOWLEDGE CHECK: Why a 2D array and not a 1D array of 81?
//  Answer: 2D array makes row/col/box indexing intuitive.
//  You CAN use 1D with index = row*9 + col (we'll do that in
//  the bitmask solver for cache efficiency!).
// ─────────────────────────────────────────────────────────────
class SudokuBoard {
public:
    // ── Constructor & Setup ────────────────────────────────
    SudokuBoard();                              // Create empty board
    SudokuBoard(const SudokuBoard& other);      // Copy constructor

    // Load a puzzle from a flat 81-character string
    // e.g., "530070000600195000..." where '0' = empty
    bool loadFromString(const std::string& puzzle_str);

    // Load puzzle from a .txt file
    bool loadFromFile(const std::string& filename);

    // Let the user type in the puzzle interactively
    void loadFromInput();

    // ── Board Access ───────────────────────────────────────
    int  getCell(int row, int col) const;
    void setCell(int row, int col, int val);
    bool isEmpty(int row, int col) const;
    bool isCellOriginal(int row, int col) const; // Is this a clue cell?

    // ── Validation (DSA: Constraint Checking) ──────────────
    // Returns true if placing 'num' at (row, col) breaks no rules
    bool isValid(int row, int col, int num) const;

    // Returns true if the entire board is correctly filled
    bool isSolved() const;

    // Returns true if the board has no rule violations so far
    bool isConsistent() const;

    // ── Display (Terminal UI) ──────────────────────────────
    void print() const;           // Colored board print
    void printPlain() const;      // Plain board (no colors)

    // ── Utility ───────────────────────────────────────────
    void reset();                 // Clear all non-original cells
    void clear();                 // Clear everything
    int  countEmpty() const;      // How many cells are blank?

    // Raw 2D grid — made public for direct solver access
    // (Solvers need fast cell access; getters would be slower)
    int grid[GRID_SIZE][GRID_SIZE];

private:
    // Tracks which cells were given as clues (original puzzle)
    // This lets us color them differently when printing.
    bool original[GRID_SIZE][GRID_SIZE];

    // ── Internal Helpers ──────────────────────────────────
    bool isValidRow(int row, int num) const;
    bool isValidCol(int col, int num) const;
    bool isValidBox(int boxRow, int boxCol, int num) const;
};
