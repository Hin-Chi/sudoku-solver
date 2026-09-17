// ============================================================
//  solver.cpp  —  All Three Solving Algorithms
//  DSA Sudoku Solver  |  MSc IT Minor Project
// ============================================================
#include "solver.h"
#include "utils.h"

#include <climits>
#include <cstring>

// ╔══════════════════════════════════════════════════════════╗
//  ALGORITHM 1: NAIVE BACKTRACKING
// ╠══════════════════════════════════════════════════════════╣
//
//  Backtracking is a general algorithm for solving constraint
//  satisfaction problems (CSPs). It builds the solution
//  incrementally and abandons a partial solution ("backtracks")
//  as soon as it determines the partial solution cannot possibly
//  lead to a valid complete solution.
//
//  PSEUDOCODE:
//    function solve(board):
//      find next empty cell (r, c)
//      if no empty cell → puzzle solved!
//      for digit d = 1 to 9:
//        if placing d at (r, c) is valid:
//          board[r][c] = d          ← choose
//          if solve(board) → return true
//          board[r][c] = 0          ← un-choose (backtrack)
//      return false                 ← no digit works → backtrack
//
//  TIME COMPLEXITY: O(9^m) — exponential in number of empty cells
//  SPACE COMPLEXITY: O(m) — recursion stack depth
// ╚══════════════════════════════════════════════════════════╝

SolveResult Solver::solveNaive(SudokuBoard& board) {
    Timer t;
    long long steps = 0;

    t.start();
    bool ok = naiveBacktrack(board, steps);
    t.stop();

    return { ok, steps, t.elapsed_us() };
}

// The actual recursive backtracking function
bool Solver::naiveBacktrack(SudokuBoard& board, long long& steps) {
    steps++; // Count every recursive call to measure complexity

    // STEP 1: Find the next empty cell (scan row by row)
    int emptyRow = -1, emptyCol = -1;
    for (int r = 0; r < GRID_SIZE && emptyRow == -1; r++) {
        for (int c = 0; c < GRID_SIZE && emptyRow == -1; c++) {
            if (board.isEmpty(r, c)) {
                emptyRow = r;
                emptyCol = c;
            }
        }
    }

    // BASE CASE: No empty cell found → puzzle is solved!
    if (emptyRow == -1) return true;

    // STEP 2: Try placing digits 1 through 9
    for (int digit = 1; digit <= 9; digit++) {

        // Check all three constraints (row, col, box)
        // isValid() scans O(9) cells for each constraint → O(27) total
        if (board.isValid(emptyRow, emptyCol, digit)) {

            // CHOOSE: Place the digit
            board.setCell(emptyRow, emptyCol, digit);

            // EXPLORE: Recurse — try to solve the rest of the board
            if (naiveBacktrack(board, steps)) {
                return true; // Propagate success up the call stack
            }

            // UN-CHOOSE: This digit didn't lead to a solution.
            // Remove it so we can try the next digit.
            // This is the "backtrack" step — hence the name!
            board.setCell(emptyRow, emptyCol, EMPTY_CELL);
        }
    }

    // No digit (1–9) worked at this cell → signal failure to parent
    return false;
}

// ╔══════════════════════════════════════════════════════════╗
//  ALGORITHM 2: BITMASK BACKTRACKING
// ╠══════════════════════════════════════════════════════════╣
//
//  DSA CONCEPT: BIT MANIPULATION / BITMASK
//
//  PROBLEM with Algorithm 1:
//    isValid() scans the entire row, column, and box to check
//    if a digit is already used. That's O(9+9+9) = O(27) per
//    placement attempt. With millions of recursive calls, this
//    adds up significantly!
//
//  SOLUTION: Use a bitmask (integer where each BIT represents
//    whether a digit is used).
//
//  HOW BITMASKS WORK HERE:
//    rowMask[r] is a 9-bit integer.
//    If bit k is SET (1), digit (k+1) is already in row r.
//    If bit k is CLEAR (0), digit (k+1) is not yet in row r.
//
//    Example: row 0 has digits 5, 3, 7
//    Bit positions (0-indexed): digit d uses bit (d-1)
//    Digit 3 → bit 2 → 0b000000100
//    Digit 5 → bit 4 → 0b000010000
//    Digit 7 → bit 6 → 0b001000000
//    Combined rowMask[0] = 0b001010100 = 84
//
//  CHECK if digit d is safe in row r:
//    (rowMask[r] & (1 << (d-1))) == 0
//    This is O(1)! No loop needed.
//
//  PLACE digit d in row r:
//    rowMask[r] |= (1 << (d-1))    ← set bit (OR)
//
//  REMOVE digit d from row r (backtrack):
//    rowMask[r] &= ~(1 << (d-1))   ← clear bit (AND with NOT)
//
//  SPEEDUP: Validity check: O(27) → O(1)  ✓
// ╚══════════════════════════════════════════════════════════╝

// Build bitmasks from the current board state
void Solver::initMasks(const SudokuBoard& board) {
    memset(rowMask, 0, sizeof(rowMask));
    memset(colMask, 0, sizeof(colMask));
    memset(boxMask, 0, sizeof(boxMask));

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            int val = board.grid[r][c];
            if (val != EMPTY_CELL) {
                int bit = 1 << (val - 1);   // Bit position for this digit
                rowMask[r] |= bit;
                colMask[c] |= bit;
                boxMask[boxIndex(r, c)] |= bit;
            }
        }
    }
}

SolveResult Solver::solveBitmask(SudokuBoard& board) {
    Timer t;
    long long steps = 0;

    initMasks(board);   // Build bitmasks once before solving
    t.start();
    bool ok = bitmaskBacktrack(board, steps);
    t.stop();

    return { ok, steps, t.elapsed_us() };
}

bool Solver::bitmaskBacktrack(SudokuBoard& board, long long& steps) {
    steps++;

    // Find next empty cell
    int emptyRow = -1, emptyCol = -1;
    for (int r = 0; r < GRID_SIZE && emptyRow == -1; r++)
        for (int c = 0; c < GRID_SIZE && emptyRow == -1; c++)
            if (board.isEmpty(r, c)) { emptyRow = r; emptyCol = c; }

    if (emptyRow == -1) return true; // Solved!

    int box = boxIndex(emptyRow, emptyCol);

    // Compute which digits are already forbidden at this cell
    // by OR-ing the three masks together.
    // usedBits = set of all digits taken by row, col, or box.
    int usedBits = rowMask[emptyRow] | colMask[emptyCol] | boxMask[box];

    for (int digit = 1; digit <= 9; digit++) {
        int bit = 1 << (digit - 1);

        // O(1) check: is this bit set in usedBits? If yes, skip.
        if (usedBits & bit) continue; // Digit already used — skip

        // CHOOSE: Place digit and update masks
        board.grid[emptyRow][emptyCol] = digit;
        rowMask[emptyRow] |= bit;
        colMask[emptyCol] |= bit;
        boxMask[box]      |= bit;

        // EXPLORE
        if (bitmaskBacktrack(board, steps)) return true;

        // UN-CHOOSE: Restore board and masks (backtrack)
        board.grid[emptyRow][emptyCol] = EMPTY_CELL;
        rowMask[emptyRow] &= ~bit; // Clear bit using AND-NOT
        colMask[emptyCol] &= ~bit;
        boxMask[box]      &= ~bit;
    }

    return false;
}

// ╔══════════════════════════════════════════════════════════╗
//  ALGORITHM 3: MRV HEURISTIC + FORWARD CHECKING
// ╠══════════════════════════════════════════════════════════╣
//
//  DSA CONCEPTS: Heuristics, Greedy Selection, Constraint Propagation
//
//  PROBLEM with Algorithms 1 & 2:
//    We always pick the FIRST empty cell (top-left scan).
//    But some cells are much more constrained than others!
//    A cell with only 1 legal digit should be filled first —
//    it has only ONE choice, so no branching occurs.
//    A cell with 8 legal digits causes 8-way branching.
//
//  SOLUTION — MRV (Minimum Remaining Values) Heuristic:
//    Always pick the empty cell that has the FEWEST legal
//    digits. This minimizes branching and detects failures
//    early (a cell with 0 legal values means we must backtrack
//    immediately, without trying anything).
//
//  FORWARD CHECKING:
//    When we place a digit at (r, c), we immediately remove
//    that digit from the domains of all cells in the same
//    row, column, and box. If any cell's domain becomes empty,
//    we know we've reached a dead end → backtrack instantly.
//
//  COMBINATION EFFECT:
//    MRV + Forward Checking typically reduces the number of
//    recursive calls by 10x–1000x compared to naive backtracking,
//    especially on hard puzzles!
// ╚══════════════════════════════════════════════════════════╝

// Build the domain (possible values) for every cell
// domain[r][c] is a 9-bit mask of legal digits for cell (r, c)
void Solver::initDomains(const SudokuBoard& board) {
    // First, rebuild the bitmasks so we can compute domains
    initMasks(board);

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (board.grid[r][c] != EMPTY_CELL) {
                domain[r][c] = 0; // Fixed cell — no choices
            } else {
                // All 9 digits are possible initially: 0b111111111 = 511
                int all = (1 << 9) - 1;
                // Remove digits already used in row, col, box
                int used = rowMask[r] | colMask[c] | boxMask[boxIndex(r, c)];
                domain[r][c] = all & ~used; // Remaining legal digits
            }
        }
    }
}

// Pick the empty cell with the fewest legal values (MRV)
std::pair<int,int> Solver::pickMRVCell(const SudokuBoard& board) {
    int bestRow = -1, bestCol = -1;
    int bestCount = INT_MAX;

    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (board.grid[r][c] != EMPTY_CELL) continue;

            // __builtin_popcount counts the number of set bits (1s)
            // This tells us how many legal digits remain.
            int count = __builtin_popcount(domain[r][c]);

            // Return immediately if we find a cell with 0 choices
            // (it's already a dead end — return it so we backtrack fast)
            if (count == 0) return {r, c};

            if (count < bestCount) {
                bestCount = count;
                bestRow = r;
                bestCol = c;
            }
        }
    }
    return {bestRow, bestCol};
}

SolveResult Solver::solveMRV(SudokuBoard& board) {
    Timer t;
    long long steps = 0;

    initDomains(board); // Build domains from current board
    t.start();
    bool ok = mrvBacktrack(board, steps);
    t.stop();

    return { ok, steps, t.elapsed_us() };
}

bool Solver::mrvBacktrack(SudokuBoard& board, long long& steps) {
    steps++;

    // Pick the most constrained cell (MRV heuristic)
    auto [row, col] = pickMRVCell(board);

    // BASE CASE: No empty cell found → solved!
    if (row == -1) return true;

    // Dead end: this cell has no legal values
    if (domain[row][col] == 0) return false;

    int box = boxIndex(row, col);
    int savedDomain[GRID_SIZE][GRID_SIZE]; // Save domains for backtracking
    memcpy(savedDomain, domain, sizeof(domain));

    // Iterate over each possible digit in this cell's domain
    int domainCopy = domain[row][col];
    while (domainCopy) {
        // Extract lowest set bit → the smallest available digit's bit
        int bit = domainCopy & (-domainCopy);
        domainCopy &= domainCopy - 1; // Remove lowest bit

        // Convert bit position back to digit value (1–9)
        int digit = 0;
        int tmp = bit;
        while (tmp > 1) { tmp >>= 1; digit++; }
        digit++; // digit = log2(bit) + 1

        // CHOOSE: Place digit, update masks and domains
        board.grid[row][col] = digit;
        rowMask[row] |= bit;
        colMask[col] |= bit;
        boxMask[box] |= bit;

        // FORWARD CHECKING: Propagate constraint to peers
        // Remove 'digit' from domains of all cells in same row, col, box
        bool valid = true;
        for (int i = 0; i < GRID_SIZE && valid; i++) {
            // Same row
            if (board.grid[row][i] == EMPTY_CELL) {
                domain[row][i] &= ~bit;
                // If a peer has no options left, this placement is invalid
                if (domain[row][i] == 0) valid = false;
            }
            // Same column
            if (board.grid[i][col] == EMPTY_CELL) {
                domain[i][col] &= ~bit;
                if (domain[i][col] == 0) valid = false;
            }
        }
        // Same 3×3 box
        int br = (row / BOX_SIZE) * BOX_SIZE;
        int bc = (col / BOX_SIZE) * BOX_SIZE;
        for (int r = br; r < br + BOX_SIZE && valid; r++) {
            for (int c = bc; c < bc + BOX_SIZE && valid; c++) {
                if (board.grid[r][c] == EMPTY_CELL) {
                    domain[r][c] &= ~bit;
                    if (domain[r][c] == 0) valid = false;
                }
            }
        }
        domain[row][col] = 0; // This cell is now filled

        // EXPLORE: Only recurse if forward checking passed
        if (valid && mrvBacktrack(board, steps)) return true;

        // UN-CHOOSE: Restore everything (backtrack)
        board.grid[row][col] = EMPTY_CELL;
        rowMask[row] &= ~bit;
        colMask[col] &= ~bit;
        boxMask[box] &= ~bit;
        memcpy(domain, savedDomain, sizeof(domain)); // Restore all domains
    }

    return false;
}
