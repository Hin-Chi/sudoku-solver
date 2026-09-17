// ============================================================
//  utils.h  —  Colors, Timer, and Helper Utilities
//  DSA Sudoku Solver  |  MSc IT Minor Project
// ============================================================
#pragma once

#include <iostream>
#include <chrono>
#include <string>

// ─────────────────────────────────────────────────────────────
//  ANSI COLOR CODES
//  These escape sequences change the terminal text color.
//  Works in Windows Terminal, VS Code terminal, and most modern
//  terminal emulators. If colors look wrong, use --no-color.
// ─────────────────────────────────────────────────────────────
namespace Color {
    // Reset
    const std::string RESET   = "\033[0m";

    // Text colors
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";

    // Bright variants
    const std::string BRIGHT_RED     = "\033[91m";
    const std::string BRIGHT_GREEN   = "\033[92m";
    const std::string BRIGHT_YELLOW  = "\033[93m";
    const std::string BRIGHT_BLUE    = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN    = "\033[96m";
    const std::string BRIGHT_WHITE   = "\033[97m";

    // Text styles
    const std::string BOLD      = "\033[1m";
    const std::string DIM       = "\033[2m";
    const std::string UNDERLINE = "\033[4m";
}

// ─────────────────────────────────────────────────────────────
//  HIGH-RESOLUTION TIMER
//  Uses std::chrono for microsecond-precision timing.
//  Usage:
//      Timer t;
//      t.start();
//      // ... your code ...
//      t.stop();
//      std::cout << t.elapsed_us() << " microseconds\n";
// ─────────────────────────────────────────────────────────────
class Timer {
public:
    // Record the start time
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    // Record the stop time
    void stop() {
        end_time = std::chrono::high_resolution_clock::now();
    }

    // Returns elapsed time in microseconds (µs)
    long long elapsed_us() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time).count();
    }

    // Returns elapsed time in milliseconds (ms)
    double elapsed_ms() const {
        return elapsed_us() / 1000.0;
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
};

// ─────────────────────────────────────────────────────────────
//  PRINT HELPERS
// ─────────────────────────────────────────────────────────────

// Print a horizontal separator line
inline void printLine(char ch = '-', int len = 50) {
    std::cout << Color::DIM;
    for (int i = 0; i < len; i++) std::cout << ch;
    std::cout << Color::RESET << "\n";
}

// Print a section header with title
inline void printHeader(const std::string& title) {
    std::cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN;
    std::cout << "  ╔══════════════════════════════════════════╗\n";
    std::cout << "  ║  " << title;
    // Pad to fixed width
    int pad = 40 - static_cast<int>(title.length());
    for (int i = 0; i < pad; i++) std::cout << " ";
    std::cout << "║\n";
    std::cout << "  ╚══════════════════════════════════════════╝\n";
    std::cout << Color::RESET << "\n";
}

// Print a colored label + value pair
inline void printStat(const std::string& label,
                      const std::string& value,
                      const std::string& color = Color::BRIGHT_YELLOW) {
    std::cout << Color::BOLD << "  " << label << ": "
              << Color::RESET << color << value
              << Color::RESET << "\n";
}
