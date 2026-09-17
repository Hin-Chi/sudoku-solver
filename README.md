# 🧩 Sudoku Solver — C++ DSA Minor Project

> **MSc IT · Advanced Data Structures & Algorithms**

A feature-rich Sudoku Solver built in C++ demonstrating three progressively
optimized solving algorithms, with a colorful terminal UI and performance analysis.

---

## 📚 DSA Concepts Covered

| Concept | Where Used |
|---|---|
| **Backtracking** | All three algorithms — core solving paradigm |
| **Recursion** | Recursive backtracking with explicit undo steps |
| **Constraint Satisfaction** | Sudoku rules modeled as row/col/box constraints |
| **Bit Manipulation / Bitmask** | Algorithm 2 — O(1) validity checks using integer bits |
| **Heuristics (MRV)** | Algorithm 3 — Minimum Remaining Values cell selection |
| **Constraint Propagation** | Algorithm 3 — Forward Checking to prune search space |
| **2D Array (Matrix)** | Board representation: `grid[9][9]` |

---

## 🏗️ Project Structure

```
sudoku-solver/
├── src/
│   ├── main.cpp      ← Entry point, interactive menu
│   ├── sudoku.h      ← SudokuBoard class declaration
│   ├── sudoku.cpp    ← Board: load, validate, display
│   ├── solver.h      ← Solver class declaration (3 algorithms)
│   ├── solver.cpp    ← All solving algorithms
│   └── utils.h       ← Colors, Timer, helpers
├── puzzles/
│   ├── easy.txt
│   ├── medium.txt
│   └── hard.txt
├── Makefile
└── README.md
```

---

## 🚀 How to Build & Run

### Prerequisites
- **g++ compiler** with C++17 support (MinGW on Windows)
- Make (or compile manually)

### Build with Make
```bash
make          # Compile the project
make run      # Compile and run immediately
make clean    # Remove compiled files
make debug    # Debug build with -g flag
```

### Manual Compilation (without Make)
```bash
g++ -std=c++17 -Wall -O2 -o sudoku src/main.cpp src/sudoku.cpp src/solver.cpp
./sudoku
```

---

## 🔬 The Three Algorithms

### Algorithm 1 — Naive Backtracking
- Scan for the **first empty cell** (top-left to bottom-right)
- Try digits 1–9; check validity by **scanning** row, col, box → O(27)
- Recurse; if stuck, undo and try next digit (backtrack)
- **Time Complexity:** O(9^m) where *m* = number of empty cells

### Algorithm 2 — Bitmask Optimization
- Same backtracking logic, but validity is checked in **O(1)**
- Uses three integer arrays (`rowMask`, `colMask`, `boxMask`)
- Each integer's bits represent which digits are already placed
- Place: `mask |= (1 << (d-1))`  |  Remove: `mask &= ~(1 << (d-1))`

### Algorithm 3 — MRV Heuristic + Forward Checking
- **MRV:** Instead of picking the first empty cell, pick the cell
  with the **fewest remaining legal digits** (Minimum Remaining Values)
- **Forward Checking:** After placing a digit, immediately eliminate it
  from all peer cells' domains; if any domain empties → backtrack instantly
- Dramatically reduces the search tree, especially on hard puzzles

---

## 📊 Expected Performance (Hard Puzzle)

| Algorithm | Recursive Steps | Typical Time |
|---|---|---|
| Naive Backtracking | ~10,000,000+ | ~500 ms |
| Bitmask Optimization | ~10,000,000+ | ~50 ms |
| MRV Heuristic | ~200–2,000 | < 1 ms |

> MRV can be **1000x fewer steps** than naive on hard puzzles!

---

## 🖥️ Features

- **Interactive menu** — load built-in puzzles or enter your own
- **Color-coded board** — clue cells in cyan, solved cells in green
- **Algorithm comparison** — side-by-side steps and time table
- **DSA concepts explainer** — built-in educational content
- **File loading** — load custom puzzles from `.txt` files
- **Performance timer** — microsecond-precision measurement

---

## 📝 Sample Puzzle (Easy)

```
    ┌───────┬───────┬───────┐
    │ 5 3 . │ . 7 . │ . . . │
    │ 6 . . │ 1 9 5 │ . . . │
    │ . 9 8 │ . . . │ . 6 . │
    ├───────┼───────┼───────┤
    │ 8 . . │ . 6 . │ . . 3 │
    │ 4 . . │ 8 . 3 │ . . 1 │
    │ 7 . . │ . 2 . │ . . 6 │
    ├───────┼───────┼───────┤
    │ . 6 . │ . . . │ 2 8 . │
    │ . . . │ 4 1 9 │ . . 5 │
    │ . . . │ . 8 . │ . 7 9 │
    └───────┴───────┴───────┘
```

---

## 👨‍🎓 Author

**MSc IT Student — Advanced Data Structures Minor Project**  
Built with C++17 · No external dependencies

---

## 📖 References

1. Knuth, D. E. (2000). *Dancing Links*. — Advanced exact cover algorithm
2. Russell & Norvig (2020). *Artificial Intelligence: A Modern Approach* — MRV Heuristic
3. Cormen et al. *Introduction to Algorithms* — Backtracking & Recursion
