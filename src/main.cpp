// ============================================================
//  main.cpp  —  Interactive Menu & Entry Point
//  DSA Sudoku Solver  |  MSc IT Minor Project
// ============================================================
#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

#include "sudoku.h"
#include "solver.h"
#include "utils.h"

// ─────────────────────────────────────────────────────────────
//  BUILT-IN PUZZLE LIBRARY (81-char strings, '0' = empty)
//  Source: classic Sudoku puzzle collections
// ─────────────────────────────────────────────────────────────
namespace Puzzles {
    // Easy: ~36 clues given
    const std::string EASY =
        "530070000"
        "600195000"
        "098000060"
        "800060003"
        "400803001"
        "700020006"
        "060000280"
        "000419005"
        "000080079";

    // Medium: ~30 clues given
    const std::string MEDIUM =
        "010020300"
        "900060010"
        "002003065"
        "070800100"
        "600050009"
        "008009030"
        "780400200"
        "040070008"
        "003010040";

    // Hard: ~22 clues given (requires deep search)
    const std::string HARD =
        "800000000"
        "003600000"
        "070090200"
        "060005030"
        "004010600"
        "090400010"
        "000006080"
        "300040070"
        "005200690";

    // Evil: ~17 clues — minimum clues for a unique solution
    const std::string EVIL =
        "000000000"
        "000003085"
        "001070600"
        "000507000"
        "004000100"
        "090000000"
        "500000073"
        "002010000"
        "000040009";
}

// ─────────────────────────────────────────────────────────────
//  FORWARD DECLARATIONS
// ─────────────────────────────────────────────────────────────
void showBanner();
void showMenu();
void runSolver(SudokuBoard& board);
void runCompare(SudokuBoard& board);
void loadPuzzleMenu(SudokuBoard& board);
void showAlgorithmInfo();
int  getMenuChoice(int min, int max);

// ─────────────────────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────────────────────
int main() {
    // Enable ANSI escape codes on Windows (required for colors)
#ifdef _WIN32
    system("chcp 65001 > nul");   // UTF-8 code page for box-drawing chars
    system(""); // Enable ANSI virtual terminal processing via empty system call
#endif

    showBanner();

    SudokuBoard board;
    // Load the easy puzzle by default so the user has something
    // to play with right away.
    board.loadFromString(Puzzles::EASY);

    int choice = 0;
    while (choice != 7) {
        showMenu();
        choice = getMenuChoice(1, 7);

        switch (choice) {
            case 1: loadPuzzleMenu(board); break;
            case 2:
                std::cout << Color::BOLD << Color::BRIGHT_CYAN
                          << "\n  Current Puzzle:\n" << Color::RESET;
                board.print();
                break;
            case 3: runSolver(board);  break;
            case 4: runCompare(board); break;
            case 5: showAlgorithmInfo(); break;
            case 6:
                board.reset();
                std::cout << Color::BRIGHT_GREEN
                          << "\n  ✓ Board reset to original puzzle.\n"
                          << Color::RESET;
                break;
            case 7:
                std::cout << Color::BRIGHT_MAGENTA
                          << "\n  Thank you! Happy solving! 🧩\n\n"
                          << Color::RESET;
                break;
        }
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────
//  BANNER
// ─────────────────────────────────────────────────────────────
void showBanner() {
    std::cout << Color::BOLD << Color::BRIGHT_CYAN;
    std::cout << "\n";
    std::cout << "  ╔═══════════════════════════════════════════════╗\n";
    std::cout << "  ║                                               ║\n";
    std::cout << "  ║    ██████  ██    ██ ██████   ██████  ██  ██  ║\n";
    std::cout << "  ║    ██      ██    ██ ██   ██ ██    ██ ██ ██   ║\n";
    std::cout << "  ║    ███████ ██    ██ ██   ██ ██    ██ ████    ║\n";
    std::cout << "  ║         ██ ██    ██ ██   ██ ██    ██ ██ ██   ║\n";
    std::cout << "  ║    ██████   ██████  ██████   ██████  ██  ██  ║\n";
    std::cout << "  ║                                               ║\n";
    std::cout << "  ║         S  O  L  V  E  R                     ║\n";
    std::cout << "  ║                                               ║\n";
    std::cout << "  ║    MSc IT · Advanced Data Structures Project  ║\n";
    std::cout << "  ╚═══════════════════════════════════════════════╝\n";
    std::cout << Color::RESET << "\n";

    std::cout << Color::DIM
              << "  Algorithms: Backtracking · Bitmask · MRV Heuristic\n"
              << Color::RESET << "\n";
}

// ─────────────────────────────────────────────────────────────
//  MENU
// ─────────────────────────────────────────────────────────────
void showMenu() {
    std::cout << "\n" << Color::BOLD << Color::BRIGHT_YELLOW
              << "  ┌─────────────────────────────────┐\n"
              << "  │           MAIN MENU             │\n"
              << "  └─────────────────────────────────┘\n"
              << Color::RESET;
    std::cout << Color::BRIGHT_WHITE
              << "  1. " << Color::RESET << "Load / Change Puzzle\n"
              << Color::BRIGHT_WHITE
              << "  2. " << Color::RESET << "Display Current Puzzle\n"
              << Color::BRIGHT_WHITE
              << "  3. " << Color::RESET << "Solve (choose algorithm)\n"
              << Color::BRIGHT_WHITE
              << "  4. " << Color::RESET << "Compare All Algorithms\n"
              << Color::BRIGHT_WHITE
              << "  5. " << Color::RESET << "Algorithm Info (DSA Concepts)\n"
              << Color::BRIGHT_WHITE
              << "  6. " << Color::RESET << "Reset Board\n"
              << Color::BRIGHT_WHITE
              << "  7. " << Color::RESET << "Exit\n\n";
}

// ─────────────────────────────────────────────────────────────
//  PUZZLE LOADER
// ─────────────────────────────────────────────────────────────
void loadPuzzleMenu(SudokuBoard& board) {
    printHeader("LOAD PUZZLE");
    std::cout << "  1. Easy   (built-in)\n"
              << "  2. Medium (built-in)\n"
              << "  3. Hard   (built-in)\n"
              << "  4. Evil   (built-in, ~17 clues)\n"
              << "  5. Load from file\n"
              << "  6. Enter manually\n\n";

    int choice = getMenuChoice(1, 6);
    board.clear();

    switch (choice) {
        case 1: board.loadFromString(Puzzles::EASY);   break;
        case 2: board.loadFromString(Puzzles::MEDIUM); break;
        case 3: board.loadFromString(Puzzles::HARD);   break;
        case 4: board.loadFromString(Puzzles::EVIL);   break;
        case 5: {
            std::cout << "  Enter filename (e.g., puzzles/easy.txt): ";
            std::string fname;
            std::cin >> fname;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            board.loadFromFile(fname);
            break;
        }
        case 6:
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            board.loadFromInput();
            break;
    }

    std::cout << "\n  Loaded puzzle:\n";
    board.print();
    printStat("Empty cells", std::to_string(board.countEmpty()));
}

// ─────────────────────────────────────────────────────────────
//  RUN A SINGLE SOLVER
// ─────────────────────────────────────────────────────────────
void runSolver(SudokuBoard& board) {
    printHeader("SOLVE PUZZLE");
    std::cout << "  Choose algorithm:\n"
              << "  1. Algorithm 1 — Naive Backtracking\n"
              << "  2. Algorithm 2 — Bitmask Optimization\n"
              << "  3. Algorithm 3 — MRV Heuristic (Fastest)\n\n";

    int choice = getMenuChoice(1, 3);

    // Work on a copy so the original remains intact for comparison
    SudokuBoard copy(board);
    Solver solver;
    SolveResult result = {false, 0, 0};

    std::cout << "\n  Solving...\n\n";

    switch (choice) {
        case 1:
            result = solver.solveNaive(copy);
            printStat("Algorithm", "Naive Backtracking", Color::BRIGHT_BLUE);
            break;
        case 2:
            result = solver.solveBitmask(copy);
            printStat("Algorithm", "Bitmask Optimization", Color::BRIGHT_MAGENTA);
            break;
        case 3:
            result = solver.solveMRV(copy);
            printStat("Algorithm", "MRV Heuristic", Color::BRIGHT_GREEN);
            break;
    }

    if (result.solved) {
        copy.print();
        printStat("Status",  "✓ SOLVED", Color::BRIGHT_GREEN);
        printStat("Steps",   std::to_string(result.steps) + " recursive calls");
        printStat("Time",    std::to_string(result.time_us) + " µs  (" +
                             std::to_string(result.time_us / 1000.0) + " ms)");
    } else {
        std::cout << Color::BRIGHT_RED << "\n  ✗ No solution found!\n" << Color::RESET;
    }
}

// ─────────────────────────────────────────────────────────────
//  COMPARE ALL THREE ALGORITHMS SIDE BY SIDE
// ─────────────────────────────────────────────────────────────
void runCompare(SudokuBoard& board) {
    printHeader("ALGORITHM COMPARISON");
    std::cout << "  Running all 3 algorithms on the same puzzle...\n\n";

    Solver solver;
    SolveResult results[3];
    const char* names[] = {
        "Naive Backtracking    ",
        "Bitmask Optimization  ",
        "MRV Heuristic         "
    };

    // Run each algorithm on a fresh copy of the board
    {
        SudokuBoard c1(board);
        results[0] = solver.solveNaive(c1);
    }
    {
        SudokuBoard c2(board);
        results[1] = solver.solveBitmask(c2);
    }
    {
        SudokuBoard c3(board);
        results[2] = solver.solveMRV(c3);
    }

    // Print comparison table
    std::cout << Color::BOLD << Color::BRIGHT_YELLOW;
    std::cout << "  ┌──────────────────────────┬──────────────┬────────────┐\n";
    std::cout << "  │ Algorithm                │ Steps        │ Time (µs)  │\n";
    std::cout << "  ├──────────────────────────┼──────────────┼────────────┤\n";
    std::cout << Color::RESET;

    const std::string rowColors[] = {
        Color::BRIGHT_BLUE, Color::BRIGHT_MAGENTA, Color::BRIGHT_GREEN
    };

    for (int i = 0; i < 3; i++) {
        std::cout << rowColors[i];
        std::cout << "  │ " << std::left << std::setw(24) << names[i] << " │ ";
        std::cout << std::right << std::setw(12) << results[i].steps << " │ ";
        std::cout << std::setw(10) << results[i].time_us << " │\n";
        std::cout << Color::RESET;
    }

    std::cout << Color::BOLD << Color::BRIGHT_YELLOW;
    std::cout << "  └──────────────────────────┴──────────────┴────────────┘\n";
    std::cout << Color::RESET;

    // Show speedup ratios
    if (results[0].steps > 0) {
        double stepSpeedupBitmask = (double)results[0].steps / results[1].steps;
        double stepSpeedupMRV     = (double)results[0].steps / results[2].steps;
        std::cout << "\n";
        std::cout << Color::BRIGHT_CYAN << "  📊 Speedup Analysis (vs Naive):\n" << Color::RESET;
        std::cout << "  Bitmask reduced recursive steps by " << Color::BRIGHT_YELLOW
                  << std::fixed << std::setprecision(1) << stepSpeedupBitmask << "x"
                  << Color::RESET << "\n";
        std::cout << "  MRV reduced recursive steps by     " << Color::BRIGHT_GREEN
                  << std::fixed << std::setprecision(1) << stepSpeedupMRV << "x"
                  << Color::RESET << "\n";
    }

    // Show the final solved board (from MRV, fastest)
    if (results[2].solved) {
        SudokuBoard solved(board);
        solver.solveMRV(solved);
        std::cout << "\n  Solution:\n";
        solved.print();
    }
}

// ─────────────────────────────────────────────────────────────
//  ALGORITHM INFO / DSA CONCEPTS EXPLAINER
// ─────────────────────────────────────────────────────────────
void showAlgorithmInfo() {
    printHeader("DSA CONCEPTS EXPLAINED");

    std::cout << Color::BOLD << Color::BRIGHT_BLUE
              << "  ① Naive Backtracking\n" << Color::RESET;
    std::cout << Color::DIM
              << "  ─────────────────────────────────────────────\n"
              << Color::RESET;
    std::cout << "  • General paradigm: Build solution incrementally,\n"
              << "    abandon partial solution when constraint is violated.\n"
              << "  • Empty cell selection: First empty cell (top-left scan)\n"
              << "  • Validity check: Scans row + col + box → O(27) per check\n"
              << "  • Time Complexity: O(9^m) | Space: O(m) where m = empty cells\n\n";

    std::cout << Color::BOLD << Color::BRIGHT_MAGENTA
              << "  ② Bitmask Optimization\n" << Color::RESET;
    std::cout << Color::DIM
              << "  ─────────────────────────────────────────────\n"
              << Color::RESET;
    std::cout << "  • Uses an integer's bits as a boolean set.\n"
              << "  • rowMask[r] bit k = 1 means digit (k+1) is in row r.\n"
              << "  • Validity check: (rowMask | colMask | boxMask) & bit → O(1)\n"
              << "  • Update: OR to place, AND-NOT to remove → O(1)\n"
              << "  • Same backtracking logic, but much faster per step.\n\n";

    std::cout << Color::BOLD << Color::BRIGHT_GREEN
              << "  ③ MRV Heuristic + Forward Checking\n" << Color::RESET;
    std::cout << Color::DIM
              << "  ─────────────────────────────────────────────\n"
              << Color::RESET;
    std::cout << "  • MRV: Pick the cell with fewest remaining legal values.\n"
              << "    → Detects dead ends early, minimizes branching factor.\n"
              << "  • Forward Checking: After placing a digit, immediately\n"
              << "    remove it from domains of peers (same row/col/box).\n"
              << "    → If any peer's domain becomes empty → backtrack NOW.\n"
              << "  • Combines both ideas for maximum pruning power.\n"
              << "  • Can reduce steps by 100x–1000x on hard puzzles!\n\n";

    printLine();
    std::cout << Color::BRIGHT_YELLOW
              << "  💡 Key Insight:\n" << Color::RESET
              << "  All three algorithms solve the SAME problem correctly.\n"
              << "  The difference is HOW MANY STATES they explore.\n"
              << "  Better heuristics → fewer states → faster solution!\n\n";
}

// ─────────────────────────────────────────────────────────────
//  INPUT HELPER
// ─────────────────────────────────────────────────────────────
int getMenuChoice(int min, int max) {
    int choice = -1;
    while (choice < min || choice > max) {
        std::cout << Color::BRIGHT_WHITE << "  Enter choice [" << min << "-" << max << "]: " << Color::RESET;
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
            std::cout << Color::RED << "  Invalid input. Try again.\n" << Color::RESET;
        }
    }
    return choice;
}
