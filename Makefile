# ============================================================
#  Makefile — Sudoku Solver Build System
#  DSA Sudoku Solver | MSc IT Minor Project
#
#  HOW TO USE:
#    make          → Build the project (creates sudoku.exe)
#    make run      → Build and run
#    make clean    → Delete compiled files
#    make debug    → Build with debug symbols (for gdb)
# ============================================================

# ── Compiler & Flags ─────────────────────────────────────────
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Debug flags (used with `make debug`)
DBGFLAGS := -std=c++17 -Wall -Wextra -g -O0 -DDEBUG

# ── Directories ───────────────────────────────────────────────
SRCDIR := src
OBJDIR := obj

# ── Files ────────────────────────────────────────────────────
# Find all .cpp files in src/
SOURCES := $(wildcard $(SRCDIR)/*.cpp)

# Generate object file names: src/foo.cpp → obj/foo.o
OBJECTS := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# Output executable name
TARGET  := sudoku

# ── Default Target ───────────────────────────────────────────
all: $(OBJDIR) $(TARGET)

# ── Create obj/ directory if it doesn't exist ────────────────
$(OBJDIR):
	mkdir -p $(OBJDIR)

# ── Link: combine all .o files into the final executable ─────
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo ""
	@echo "  ✓ Build successful! Run with: ./$(TARGET)"
	@echo ""

# ── Compile: each .cpp → its .o file ─────────────────────────
# $< = the source file (prerequisite)
# $@ = the target object file
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Run ──────────────────────────────────────────────────────
run: all
	./$(TARGET)

# ── Debug Build ───────────────────────────────────────────────
debug: CXXFLAGS := $(DBGFLAGS)
debug: clean all
	@echo "  Debug build complete. Run: gdb ./$(TARGET)"

# ── Clean ─────────────────────────────────────────────────────
clean:
	rm -rf $(OBJDIR) $(TARGET) $(TARGET).exe

# Prevent make from confusing targets with filenames
.PHONY: all run debug clean
