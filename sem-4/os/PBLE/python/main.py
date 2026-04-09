"""
main.py — Virtual Memory Translation Simulator (Python)
========================================================
Ties the TLB and two-level page table together and provides:

  • A mandatory lab test case  (virtual address 0x12345678)
  • A batch stress test        (TLB hits, misses, LRU eviction)
  • An interactive REPL        (type hex addresses and commands)

Usage:
  python main.py               Interactive mode (default)
  python main.py --test        Mandatory test case only, then exit
  python main.py --batch       Built-in stress-test suite
  python main.py --help        Print usage information
"""

import sys

from vm        import (PAGE_SIZE, TLB_SIZE,
                       L1_TABLE_ENTRIES, L2_TABLE_ENTRIES,
                       PAGE_OFFSET_BITS, INVALID_FRAME,
                       TranslationResult, TranslationStatus,
                       decompose_address)
from tlb       import TLB
from pagetable import PageTable


# ══════════════════════════════════════════════════════════════════════
#  Translation Engine
# ══════════════════════════════════════════════════════════════════════

def translate(tlb: TLB, pt: PageTable, vaddr: int) -> TranslationResult:
    """
    Perform a full virtual-to-physical address translation.

    Translation pipeline:
      1. Decompose vaddr → L1 index, L2 index, offset, VPN
      2. TLB lookup(VPN)
         ├── HIT  → use cached frame directly          (fast path)
         └── MISS → page table walk                    (slow path)
                  → TLB insert (evicting LRU if full)
      3. Detect page fault (frame == INVALID_FRAME)
      4. Physical address = frame × PAGE_SIZE + offset
    """
    vaddr &= 0xFFFFFFFF                          # clamp to 32 bits
    r = TranslationResult(virtual_addr=vaddr)

    # ── Step 1: Decompose ────────────────────────────────────────────
    r.l1_index, r.l2_index, r.offset, r.vpn = decompose_address(vaddr)

    print()
    print("────────────────────────────────────────────────────")
    print(f" Translating Virtual Address: 0x{vaddr:08X}")
    print("────────────────────────────────────────────────────")
    print("  Decomposition:")
    print("    ┌────────────────┬──────────────────────────────┐")
    print("    │ Field          │ Value                        │")
    print("    ├────────────────┼──────────────────────────────┤")
    print(f"    │ L1 Index       │ {r.l1_index:<4}  (bits[31:22])          │")
    print(f"    │ L2 Index       │ {r.l2_index:<4}  (bits[21:12])          │")
    print(f"    │ Offset         │ 0x{r.offset:03X} = {r.offset:<4}  (bits[11:0])  │")
    print(f"    │ VPN            │ 0x{r.vpn:05X} = {r.vpn:<7}              │")
    print("    └────────────────┴──────────────────────────────┘")

    # ── Step 2: TLB Lookup ───────────────────────────────────────────
    print(f"\n  [Step 2] Checking TLB for VPN 0x{r.vpn:05X} ...")
    hit, frame = tlb.lookup(r.vpn)

    if hit:
        # ── TLB HIT ──
        r.tlb_hit = True
        r.frame   = frame
        print(f"  [TLB] *** HIT *** VPN 0x{r.vpn:05X} → Frame {frame}")
    else:
        # ── TLB MISS: walk the page table ──
        print("  [TLB] *** MISS *** — walking page table...")

        # ── Step 3: Page Table Walk ──────────────────────────────────
        frame = pt.lookup(r.l1_index, r.l2_index)

        if frame is INVALID_FRAME:
            r.status = TranslationStatus.PAGE_FAULT
            return r

        r.frame = frame

        # ── Step 4: Update TLB ──────────────────────────────────────
        print(f"  [Step 4] Updating TLB: VPN 0x{r.vpn:05X} → Frame {frame}")
        tlb.insert(r.vpn, frame)

    # ── Step 5: Construct Physical Address ───────────────────────────
    r.physical_addr = r.frame * PAGE_SIZE + r.offset

    print(f"\n  [Step 5] Physical Address Calculation:")
    print(f"           Frame({r.frame}) × PageSize({PAGE_SIZE}) "
          f"+ Offset({r.offset})")
    print(f"           = {r.frame} × {PAGE_SIZE} + {r.offset} "
          f"= {r.physical_addr}")

    return r


def print_result(r: TranslationResult) -> None:
    """Pretty-print a translation result summary."""
    print()
    print("  ╔══════════════════════════════════════════╗")
    print("  ║          TRANSLATION SUMMARY             ║")
    print("  ╠══════════════════════════════════════════╣")
    print(f"  ║  Virtual Address :  0x{r.virtual_addr:08X}            ║")
    print(f"  ║  L1 Index        :  {r.l1_index:<24}  ║")
    print(f"  ║  L2 Index        :  {r.l2_index:<24}  ║")
    print(f"  ║  Offset          :  0x{r.offset:03X} ({r.offset:<18})  ║")
    print(f"  ║  VPN             :  0x{r.vpn:05X}                 ║")
    print(f"  ║  Frame Number    :  {r.frame!s:<24}  ║")
    print("  ╠══════════════════════════════════════════╣")

    if r.status == TranslationStatus.PAGE_FAULT:
        print("  ║  STATUS          :  *** PAGE FAULT ***    ║")
    else:
        tlb_str = "HIT" if r.tlb_hit else "MISS (page table walk)"
        print(f"  ║  TLB Result      :  {tlb_str:<24}  ║")
        print(f"  ║  Physical Address:  {r.physical_addr:<24}  ║")
        print(f"  ║                     (0x{r.physical_addr:08X})          ║")

    print("  ╚══════════════════════════════════════════╝")


# ══════════════════════════════════════════════════════════════════════
#  Mandatory Lab Test Case (virtual address 0x12345678)
# ══════════════════════════════════════════════════════════════════════

def run_test_case(tlb: TLB, pt: PageTable) -> None:
    """
    Mandatory lab test case for virtual address 0x12345678.

    Expected decomposition:
      VPN      = 0x12345678 >> 12  = 0x12345  (74565)
      L1 index = 0x12345   >> 10  = 72            ← matches lab spec ✓
      L2 index = 0x12345   & 3FF  = 837 (0x345)   ← PDF says 342 (typo)
      Offset   = 0x12345678 & FFF = 0x678 (1656)  ← matches lab spec ✓

    Mock mapping: VPN 0x12345 → Frame 50
    Expected physical address: 50 × 4096 + 1656 = 206456
    """
    print()
    print("╔══════════════════════════════════════════════════════════╗")
    print("║              MANDATORY LAB TEST CASE                    ║")
    print("║         Virtual Address: 0x12345678                     ║")
    print("╚══════════════════════════════════════════════════════════╝")
    print("\n  Setting up mock page table mapping...")

    TEST_VPN   = 0x12345678 >> PAGE_OFFSET_BITS    # = 0x12345
    TEST_FRAME = 50
    pt.map(TEST_VPN, TEST_FRAME)

    print("\n  Running translation for 0x12345678 ...")
    r = translate(tlb, pt, 0x12345678)
    print_result(r)

    # ── Verification ─────────────────────────────────────────────────
    checks = [
        ("L1 Index",      r.l1_index,      72,     f"{r.l1_index}"),
        ("L2 Index",      r.l2_index,      837,    f"{r.l2_index}"),
        ("Offset",        r.offset,        0x678,  f"0x{r.offset:03X}"),
        ("Phys Addr",     r.physical_addr, 206456, f"{r.physical_addr}"),
    ]

    print()
    print("  ┌──────────────── Verification ────────────────────────┐")
    all_pass = True
    for name, got, expected, display in checks:
        ok = got == expected
        if not ok:
            all_pass = False
        status = "[PASS]" if ok else "[FAIL]"
        exp_str = f"0x{expected:03X}" if name == "Offset" else str(expected)
        print(f"  │  {name:<12} expected: {exp_str:<7}  got: {display:<7}  {status}  │")
    print("  └───────────────────────────────────────────────────────┘")
    print()
    print("  NOTE: Lab PDF cites L2 = 342 — correct value is 837 (0x345).")
    print("        Physical address 206456 matches the spec exactly.")
    print(f"\n  Overall: {'ALL PASS ✓' if all_pass else 'SOME FAILURES ✗'}")


# ══════════════════════════════════════════════════════════════════════
#  Batch Stress Test
# ══════════════════════════════════════════════════════════════════════

def run_batch_tests(tlb: TLB, pt: PageTable) -> None:
    """
    Translate a set of pre-mapped virtual addresses to demonstrate:
      - Cold TLB misses on first access to each VPN
      - Warm TLB hits on repeated access to the same VPN
      - LRU eviction once TLB_SIZE unique VPNs have been loaded
    """
    print()
    print("╔══════════════════════════════════════════════════════════╗")
    print("║                 BATCH STRESS TEST SUITE                 ║")
    print("╚══════════════════════════════════════════════════════════╝")

    # Install TLB_SIZE + 4 mappings, VPN spaced 128 pages apart
    n_maps = TLB_SIZE + 4
    print(f"\n  Installing {n_maps} page table mappings...")
    for i in range(n_maps):
        vpn = (i * 128) & 0xFFFFF
        pt.map(vpn, 100 + i)

    tests = [
        (0x00000100, "First  access (cold miss, VPN=0x00000)"),
        (0x00000200, "Second access (TLB HIT,  VPN=0x00000)"),
        (0x00080100, "VPN=0x00080 (cold miss)"),
        (0x00100100, "VPN=0x00100 (cold miss)"),
        (0x00180100, "VPN=0x00180 (cold miss)"),
        (0x00200100, "VPN=0x00200 (cold miss)"),
        (0x00280100, "VPN=0x00280 (cold miss)"),
        (0x00300100, "VPN=0x00300 (cold miss)"),
        (0x00380100, "VPN=0x00380 (cold miss)"),
        (0x00400100, "VPN=0x00400 (cold miss)"),
        (0x00480100, "VPN=0x00480 (cold miss)"),
        (0x00500100, "VPN=0x00500 (cold miss)"),
        (0x00580100, "VPN=0x00580 (cold miss)"),
        (0x00600100, "VPN=0x00600 (cold miss)"),
        (0x00680100, "VPN=0x00680 (cold miss)"),
        (0x00700100, "VPN=0x00700 (cold miss)"),
        # TLB now full; next inserts trigger LRU eviction
        (0x00780100, "VPN=0x00780 (LRU eviction expected)"),
        (0x00800100, "VPN=0x00800 (LRU eviction expected)"),
        # Re-access the very first VPN — likely evicted
        (0x00000100, "VPN=0x00000 re-access (may be evicted)"),
    ]

    for i, (addr, label) in enumerate(tests, start=1):
        print(f"\n[Test {i:2d}/{len(tests)}] {label}")
        r = translate(tlb, pt, addr)
        print_result(r)

    tlb.dump()


# ══════════════════════════════════════════════════════════════════════
#  Interactive REPL
# ══════════════════════════════════════════════════════════════════════

def print_banner() -> None:
    print()
    print("  ╔══════════════════════════════════════════════════════════╗")
    print(f"  ║        VIRTUAL MEMORY TRANSLATION SIMULATOR             ║")
    print(f"  ║   32-bit Addr | 2-Level Page Table | TLB (LRU, sz={TLB_SIZE:2d})  ║")
    print(f"  ║   Page Size: 4 KB  |  L1: 10 bits  |  L2: 10 bits      ║")
    print("  ╚══════════════════════════════════════════════════════════╝")
    print()


def print_help() -> None:
    print("""
USAGE:
  python main.py              Interactive mode (default)
  python main.py --test       Run mandatory test case only
  python main.py --batch      Run batch stress test suite
  python main.py --help       Show this help message

INTERACTIVE COMMANDS:
  <hex_addr>                  Translate a virtual address (e.g. 0xDEADBEEF)
  map <hex_vpn> <dec_frame>   Install a page table entry manually
  stats                       Print TLB hit/miss statistics
  tlb                         Dump current TLB contents
  flush                       Invalidate all TLB entries
  help                        Show this help message
  quit / exit / q             Exit the simulator
""")


def parse_int(s: str) -> int | None:
    """Parse a string as a hex (0x...) or decimal integer."""
    s = s.strip()
    try:
        return int(s, 16) if s.lower().startswith("0x") else int(s, 0)
    except ValueError:
        return None


def repl(tlb: TLB, pt: PageTable) -> None:
    """Interactive read-eval-print loop."""
    print("\n  Entering interactive mode. Type 'help' for commands.")
    print("  Type hex addresses (e.g. 0xDEAD0000) to translate them.\n")

    while True:
        try:
            line = input("vm_sim> ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n  Goodbye.")
            break

        if not line:
            continue

        if line in ("quit", "exit", "q"):
            print("  Goodbye.")
            break

        if line == "help":
            print_help()
            continue

        if line == "stats":
            tlb.print_stats()
            continue

        if line == "tlb":
            tlb.dump()
            continue

        if line == "flush":
            tlb.flush()
            continue

        # map <hex_vpn> <frame>
        if line.startswith("map "):
            parts = line.split()
            if len(parts) == 3:
                vpn_val   = parse_int(parts[1])
                frame_val = parse_int(parts[2])
                if vpn_val is not None and frame_val is not None:
                    pt.map(vpn_val, frame_val)
                else:
                    print("  [ERROR] Usage: map <hex_vpn> <dec_or_hex_frame>")
            else:
                print("  [ERROR] Usage: map <hex_vpn> <frame_number>")
            continue

        # Try to interpret as a virtual address
        vaddr = parse_int(line)
        if vaddr is not None:
            r = translate(tlb, pt, vaddr)
            print_result(r)
        else:
            print(f"  [ERROR] Unknown command or invalid address: '{line}'")
            print("          Type 'help' for a list of commands.")


# ══════════════════════════════════════════════════════════════════════
#  Entry Point
# ══════════════════════════════════════════════════════════════════════

def main() -> None:
    print_banner()

    # Parse flags
    flag_test  = "--test"  in sys.argv
    flag_batch = "--batch" in sys.argv
    flag_help  = "--help"  in sys.argv

    if flag_help:
        print_help()
        return

    # Unknown flags
    known = {"--test", "--batch", "--help"}
    for arg in sys.argv[1:]:
        if arg not in known:
            print(f"[ERROR] Unknown flag: {arg}  (try --help)", file=sys.stderr)
            sys.exit(1)

    # Initialise subsystems
    tlb = TLB(capacity=TLB_SIZE)
    pt  = PageTable()

    print(f"[VM] Initialised: TLB capacity={TLB_SIZE}, "
          f"Page size={PAGE_SIZE} B, "
          f"L1 entries={L1_TABLE_ENTRIES}, "
          f"L2 entries={L2_TABLE_ENTRIES}")

    # Always run the mandatory test case first
    run_test_case(tlb, pt)
    tlb.print_stats()

    if flag_test:
        return      # test-only mode: done

    # Flush TLB for a clean slate before the next mode
    tlb.flush()

    if flag_batch:
        run_batch_tests(tlb, pt)
        tlb.print_stats()
        return

    # Default: interactive REPL
    repl(tlb, pt)


if __name__ == "__main__":
    main()
