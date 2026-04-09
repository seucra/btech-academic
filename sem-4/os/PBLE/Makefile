# Makefile — Virtual Memory Simulation
# ======================================
# Targets:
#   make          — build the simulator (debug build)
#   make release  — build optimised binary
#   make test     — build and run the mandatory test case
#   make batch    — build and run the batch stress test
#   make clean    — remove all generated files

CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -g
RFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -O2 -DNDEBUG
TARGET  := vm_sim
SRCS    := main.c vm.c tlb.c pagetable.c
OBJS    := $(SRCS:.c=.o)

.PHONY: all release test batch clean

# ── Default: debug build ──────────────────────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo ""
	@echo "  Build complete → ./$(TARGET)"
	@echo "  Run with:  ./$(TARGET) [--test | --batch | --help]"
	@echo ""

%.o: %.c vm.h
	$(CC) $(CFLAGS) -c -o $@ $<

# ── Release build ─────────────────────────────────────────────────────
release: $(SRCS) vm.h
	$(CC) $(RFLAGS) -o $(TARGET) $(SRCS)
	@echo "  Release build complete → ./$(TARGET)"

# ── Run mandatory test case ───────────────────────────────────────────
test: all
	@echo ""
	@echo "=== Running mandatory test case ==="
	./$(TARGET) --test

# ── Run batch stress test ─────────────────────────────────────────────
batch: all
	@echo ""
	@echo "=== Running batch stress test ==="
	./$(TARGET) --batch

# ── Cleanup ───────────────────────────────────────────────────────────
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "  Cleaned."
