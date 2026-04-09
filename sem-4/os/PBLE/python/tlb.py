"""
tlb.py — Translation Lookaside Buffer (TLB)
============================================
Fully-associative TLB with LRU eviction.

LRU Mechanism:
  Each entry records a logical timestamp.  A global clock increments on
  every access (hit or miss).  On eviction, the entry with the *smallest*
  timestamp is replaced — it is the Least Recently Used one.
"""

from dataclasses import dataclass, field
from vm import TLB_SIZE, INVALID_FRAME


# ─────────────────────── TLBEntry ─────────────────────────────────────
@dataclass
class TLBEntry:
    valid:       bool = False
    vpn:         int  = 0
    frame:       int  = 0
    lru_counter: int  = 0          # logical clock value at last use


# ─────────────────────── TLB ──────────────────────────────────────────
class TLB:
    """
    Fully-associative Translation Lookaside Buffer with LRU replacement.

    Public interface:
        lookup(vpn)          → (hit: bool, frame: int | None)
        insert(vpn, frame)   → None
        flush()              → None
        print_stats()        → None
        dump()               → None
    """

    def __init__(self, capacity: int = TLB_SIZE):
        self.capacity  = capacity
        self.entries   = [TLBEntry() for _ in range(capacity)]
        self._clock    = 0          # logical clock
        self.hits      = 0
        self.misses    = 0
        self.evictions = 0

    # ── lookup ─────────────────────────────────────────────────────────
    def lookup(self, vpn: int) -> tuple[bool, int | None]:
        """
        Check whether vpn is cached in the TLB.

        Returns:
            (True,  frame_number)  on HIT  — updates LRU timestamp
            (False, None)          on MISS
        """
        self._clock += 1

        for entry in self.entries:
            if entry.valid and entry.vpn == vpn:
                # ── HIT: refresh recency ──
                entry.lru_counter = self._clock
                self.hits += 1
                return True, entry.frame

        self.misses += 1
        return False, None

    # ── insert ─────────────────────────────────────────────────────────
    def insert(self, vpn: int, frame: int) -> None:
        """
        Install a new VPN→frame mapping.

        1. Use any invalid (empty) slot first.
        2. If TLB is full, evict the entry with the lowest lru_counter (LRU).
        """
        self._clock += 1

        # Pass 1: look for an empty slot
        for entry in self.entries:
            if not entry.valid:
                entry.valid       = True
                entry.vpn         = vpn
                entry.frame       = frame
                entry.lru_counter = self._clock
                return

        # Pass 2: LRU eviction — find minimum lru_counter
        lru_entry = min(self.entries, key=lambda e: e.lru_counter)
        print(f"  [TLB] Evicting VPN 0x{lru_entry.vpn:05X} (LRU) "
              f"→ installing VPN 0x{vpn:05X}")
        self.evictions += 1

        lru_entry.vpn         = vpn
        lru_entry.frame       = frame
        lru_entry.lru_counter = self._clock
        # lru_entry.valid stays True

    # ── flush ──────────────────────────────────────────────────────────
    def flush(self) -> None:
        """Invalidate all entries (e.g. on a context switch)."""
        for entry in self.entries:
            entry.valid = False
        print(f"  [TLB] Flushed all {self.capacity} entries.")

    # ── print_stats ────────────────────────────────────────────────────
    def print_stats(self) -> None:
        total    = self.hits + self.misses
        hit_rate = (100.0 * self.hits / total) if total else 0.0

        print()
        print("┌─────────────────────────────┐")
        print("│        TLB Statistics       │")
        print("├──────────────┬──────────────┤")
        print(f"│ Accesses     │ {total:<12} │")
        print(f"│ Hits         │ {self.hits:<12} │")
        print(f"│ Misses       │ {self.misses:<12} │")
        print(f"│ Hit Rate     │ {hit_rate:<11.2f}% │")
        print(f"│ Evictions    │ {self.evictions:<12} │")
        print("└──────────────┴──────────────┘")

    # ── dump ───────────────────────────────────────────────────────────
    def dump(self) -> None:
        """Print all valid TLB entries for debugging."""
        print(f"\n  ╔══════ TLB Contents (capacity={self.capacity}) ══════╗")
        print( "  ║  Slot │    VPN    │  Frame  │ LRU tick ║")
        print( "  ╠═══════╪═══════════╪═════════╪══════════╣")

        valid_entries = [(i, e) for i, e in enumerate(self.entries) if e.valid]
        if not valid_entries:
            print("  ║               (empty)                   ║")
        else:
            for i, e in valid_entries:
                print(f"  ║  {i:4d} │ 0x{e.vpn:07X} │ {e.frame:7d} │ {e.lru_counter:8d}  ║")

        print("  ╚═══════╧═══════════╧═════════╧══════════╝")
