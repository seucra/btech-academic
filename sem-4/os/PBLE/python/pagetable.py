"""
pagetable.py — Two-Level Page Table
=====================================
Lazy-allocated two-level page table:

  L1 Table   (list of 1024 slots, each None or an L2Table)
    └── L2 Table  (list of 1024 slots, each a frame number or None)

An L2 table is only created the first time a VPN that hashes into its
L1 slot is mapped — exactly as a real OS kernel does.
"""

from vm import (L1_TABLE_ENTRIES, L2_TABLE_ENTRIES,
                L1_INDEX_BITS, L2_INDEX_BITS,
                L1_INDEX_MASK, L2_INDEX_MASK,
                INVALID_FRAME)


class PageTable:
    """
    Two-level page table with dynamic L2 allocation.

    Public interface:
        map(vpn, frame)              → bool
        lookup(l1_idx, l2_idx)       → frame | None
    """

    def __init__(self):
        # L1: list of (L2 table | None), one slot per L1 index
        self._l1: list[list[int | None] | None] = [None] * L1_TABLE_ENTRIES

    # ── map ─────────────────────────────────────────────────────────────
    def map(self, vpn: int, frame: int) -> bool:
        """
        Install VPN → frame in the page table.

        1. Compute L1 and L2 indices from the VPN.
        2. Allocate an L2 table if this L1 slot is empty (lazy allocation).
        3. Write the frame into the appropriate L2 slot.
        """
        l1_idx = (vpn >> L2_INDEX_BITS) & L1_INDEX_MASK
        l2_idx = vpn & L2_INDEX_MASK

        # Lazy allocation: create L2 table on first use
        if self._l1[l1_idx] is None:
            self._l1[l1_idx] = [INVALID_FRAME] * L2_TABLE_ENTRIES
            print(f"  [PT] Allocated new L2 table for L1[{l1_idx}]")

        self._l1[l1_idx][l2_idx] = frame
        print(f"  [PT] Mapped VPN 0x{vpn:05X} (L1[{l1_idx}][{l2_idx}]) "
              f"→ Frame {frame}")
        return True

    # ── lookup ──────────────────────────────────────────────────────────
    def lookup(self, l1_idx: int, l2_idx: int) -> int | None:
        """
        Walk the page table and return the frame number, or None on page fault.

        Step 1 — L1 lookup: if the L1 slot is None → page fault.
        Step 2 — L2 lookup: if the L2 entry is None → page fault.
        """
        # Step 1: L1 lookup
        l2_table = self._l1[l1_idx]
        if l2_table is None:
            print(f"  [PT] L1[{l1_idx}] is None → no L2 table → PAGE FAULT")
            return INVALID_FRAME

        # Step 2: L2 lookup
        frame = l2_table[l2_idx]
        if frame is INVALID_FRAME:
            print(f"  [PT] L1[{l1_idx}][{l2_idx}] = INVALID → PAGE FAULT")
            return INVALID_FRAME

        print(f"  [PT] L1[{l1_idx}] → L2 table (id={id(l2_table):#x})")
        print(f"  [PT] L2[{l2_idx}] → Frame {frame}")
        return frame
