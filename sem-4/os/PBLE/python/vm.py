"""
vm.py — Shared Constants and Data Classes
==========================================
Architecture  : 32-bit virtual addresses
Page size     : 4 KB  (2^12 bytes → 12-bit offset)
Page table    : 2-level  (10-bit L1 + 10-bit L2)

Virtual Address Layout:
  31          22 21          12 11            0
  ┌─────────────┬─────────────┬───────────────┐
  │  L1 (10b)   │  L2 (10b)   │  Offset (12b) │
  └─────────────┴─────────────┴───────────────┘

Physical Address = Frame Number × PAGE_SIZE + Offset
"""

from dataclasses import dataclass, field
from enum import Enum, auto

# ─────────────────────── Architecture Constants ────────────────────────
PAGE_SIZE         = 4096          # 4 KB
PAGE_OFFSET_BITS  = 12
L1_INDEX_BITS     = 10
L2_INDEX_BITS     = 10

OFFSET_MASK       = (1 << PAGE_OFFSET_BITS) - 1   # 0x00000FFF
L2_INDEX_MASK     = (1 << L2_INDEX_BITS)   - 1    # 0x000003FF
L1_INDEX_MASK     = (1 << L1_INDEX_BITS)   - 1    # 0x000003FF

L1_TABLE_ENTRIES  = 1 << L1_INDEX_BITS             # 1024
L2_TABLE_ENTRIES  = 1 << L2_INDEX_BITS             # 1024

TLB_SIZE          = 16

INVALID_FRAME     = None           # page not present


# ─────────────────────── Translation Status ────────────────────────────
class TranslationStatus(Enum):
    OK         = auto()
    PAGE_FAULT = auto()


# ─────────────────────── Result Dataclass ─────────────────────────────
@dataclass
class TranslationResult:
    """Full breakdown of one address translation."""
    virtual_addr:   int = 0
    l1_index:       int = 0
    l2_index:       int = 0
    offset:         int = 0
    vpn:            int = 0
    frame:          int | None = None
    physical_addr:  int = 0
    tlb_hit:        bool = False
    status:         TranslationStatus = TranslationStatus.OK


# ─────────────────────── Address Decomposition ────────────────────────
def decompose_address(vaddr: int) -> tuple[int, int, int, int]:
    """
    Split a 32-bit virtual address into (l1_index, l2_index, offset, vpn).

    Bit layout:
      bits[31:22] → L1 index  (10 bits)
      bits[21:12] → L2 index  (10 bits)
      bits[11: 0] → Offset    (12 bits)
      VPN = bits[31:12]       (L1_index << 10 | L2_index)
    """
    vaddr    &= 0xFFFFFFFF               # clamp to 32 bits
    offset    = vaddr & OFFSET_MASK
    vpn       = vaddr >> PAGE_OFFSET_BITS
    l1_index  = (vpn >> L2_INDEX_BITS) & L1_INDEX_MASK
    l2_index  = vpn & L2_INDEX_MASK
    return l1_index, l2_index, offset, vpn
