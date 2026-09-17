// ============================================================
//  solver.h  —  Solver Class Declaration
//  DSA Sudoku Solver  |  MSc IT Minor Project
// ============================================================
//
//  We implement THREE progressively smarter solving algorithms:
//
//  Algorithm 1 — Naive Backtracking
//    Find next empty cell (top-left), try 1–9, recurse, undo.
//    Time complexity: O(9^m) where m = number of empty cells.
//
//  Algorithm 2 — Bitmask Backtracking
//    Same logic but uses bit operations to check validity in O(1)
//    instead of scanning rows/cols/boxes (O(9) each).
//    DSA Concept: Bitmasking / Bit Manipulation
//
//  Algorithm 3 — MRV + Forward Checking
//    DSA Concept: Heuristics, Constraint Propagation
//    Pick the cell with the FEWEST remaining legal values
//    (Minimum Remaining Values heuristic) — this prunes the
//    search tree drastically, especially for hard puzzles.
// ============================================================
#pragma once

#include "sudoku.h"
#include <utility>   // for std::pair

// ─────────────────────────────────────────────────────────────
//  SolveResult — returned by every solver
// ─────────────────────────────────────────────────────────────
struct SolveResult {
    bool     solved;       // Was a solution found?
    long long steps;       // How many recursive calls were made?
    long long time_us;     // How long did it take (microseconds)?
};

// ─────────────────────────────────────────────────────────────
//  Solver CLASS
// ─────────────────────────────────────────────────────────────
class Solver {
public:
    // ── Algorithm 1: Naive Backtracking ───────────────────
    // Entry point — calls the recursive helper below
    SolveResult solveNaive(SudokuBoard& board);

    // ── Algorithm 2: Bitmask Backtracking ─────────────────
    SolveResult solveBitmask(SudokuBoard& board);

    // ── Algorithm 3: MRV + Forward Checking ───────────────
    SolveResult solveMRV(SudokuBoard& board);

private:
    // ── Algorithm 1 internal ──────────────────────────────
    // Returns true when solved; 'steps' counts recursive calls
    bool naiveBacktrack(SudokuBoard& board, long long& steps);

    // ── Algorithm 2 internal ──────────────────────────────
    // Bitmask arrays (maintained during solve, then restored)
    int rowMask[GRID_SIZE]; // rowMask[r] has bit k set if digit k+1 is used in row r
    int colMask[GRID_SIZE]; // colMask[c] has bit k set if digit k+1 is used in col c
    int boxMask[GRID_SIZE]; // boxMask[b] has bit k set if digit k+1 is used in box b

    // Map a (row, col) to its box index 0–8
    // Box index = (row/3)*3 + (col/3)
    inline int boxIndex(int row, int col) const {
        return (row / BOX_SIZE) * BOX_SIZE + (col / BOX_SIZE);
    }

    void initMasks(const SudokuBoard& board); // Build masks from current board
    bool bitmaskBacktrack(SudokuBoard& board, long long& steps);

    // ── Algorithm 3 internal ──────────────────────────────
    // Possible-values domain for each cell (bitmask of digits 1-9)
    int domain[GRID_SIZE][GRID_SIZE];

    void initDomains(const SudokuBoard& board);
    // Returns {row, col} of cell with fewest legal values, or {-1,-1}
    std::pair<int,int> pickMRVCell(const SudokuBoard& board);
    bool mrvBacktrack(SudokuBoard& board, long long& steps);
};
