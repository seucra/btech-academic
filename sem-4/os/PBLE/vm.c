/*
 * vm.c — Virtual Memory Translation Engine
 * ==========================================
 * Ties together the TLB and the two-level page table to implement the
 * full virtual-to-physical address translation pipeline.
 *
 * Translation Flow for each virtual address:
 *
 *   ┌─────────────────────────────────────────────────────────┐
 *   │  1. Decompose  vaddr → L1 index, L2 index, Offset, VPN │
 *   │  2. TLB lookup(VPN)                                      │
 *   │     ├── HIT  → use cached Frame directly                 │
 *   │     └── MISS → walk page table                           │
 *   │              → TLB insert (possibly evicting LRU entry)  │
 *   │  3. Check for page fault (Frame == INVALID_FRAME)        │
 *   │  4. Physical Address = Frame × PAGE_SIZE + Offset        │
 *   └─────────────────────────────────────────────────────────┘
 */

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

/* ─────────────────────────── decompose_address ─────────────────────── */
/**
 * decompose_address — split a 32-bit virtual address into its components.
 *
 * Bit layout:
 *   bits [31:22] → L1 index (10 bits)
 *   bits [21:12] → L2 index (10 bits)
 *   bits [11: 0] → Byte offset within page (12 bits)
 *
 *   VPN = bits[31:12] (the full 20-bit virtual page number)
 *       = (L1_index << 10) | L2_index
 *
 * @param addr    32-bit virtual address
 * @param l1_idx  output: L1 index (0 – 1023)
 * @param l2_idx  output: L2 index (0 – 1023)
 * @param offset  output: byte offset within page (0 – 4095)
 * @param vpn     output: full virtual page number (0 – 2^20-1)
 */
void decompose_address(vaddr_t addr,
                       uint32_t *l1_idx,
                       uint32_t *l2_idx,
                       uint32_t *offset,
                       vpn_t    *vpn)
{
    *offset  = addr & OFFSET_MASK;                          /* bits[11: 0] */
    *vpn     = addr >> PAGE_OFFSET_BITS;                    /* bits[31:12] */
    *l1_idx  = (*vpn >> L2_INDEX_BITS) & L1_INDEX_MASK;    /* bits[31:22] */
    *l2_idx  = *vpn & L2_INDEX_MASK;                       /* bits[21:12] */
}

/* ─────────────────────────── vm_init ───────────────────────────────── */
/**
 * vm_init — initialise the TLB.
 * The page table is created externally via pt_create(); this function just
 * resets the TLB to a clean state.
 */
void vm_init(TLB *tlb, L1PageTable *l1)
{
    (void)l1;  /* page table initialised externally; parameter kept for API symmetry */
    tlb_init(tlb);
    printf("[VM] Initialised: TLB capacity=%d, Page size=%u B, "
           "L1 entries=%u, L2 entries=%u\n",
           TLB_SIZE, PAGE_SIZE, L1_TABLE_ENTRIES, L2_TABLE_ENTRIES);
}

/* ─────────────────────────── vm_translate ──────────────────────────── */
/**
 * vm_translate — perform a full virtual-to-physical translation.
 *
 * @param tlb   the TLB cache
 * @param l1    the two-level page table
 * @param vaddr 32-bit virtual address to translate
 * @return      a TranslationResult with all intermediate values populated
 */
TranslationResult vm_translate(TLB *tlb, L1PageTable *l1, vaddr_t vaddr)
{
    TranslationResult r;
    r.virtual_addr = vaddr;
    r.status       = TRANS_OK;
    r.tlb_hit      = false;

    /* ── Step 1: Decompose the virtual address ── */
    decompose_address(vaddr, &r.l1_index, &r.l2_index, &r.offset, &r.vpn);

    printf("\n────────────────────────────────────────────────────\n");
    printf(" Translating Virtual Address: 0x%08X\n", vaddr);
    printf("────────────────────────────────────────────────────\n");
    printf("  Decomposition:\n");
    printf("    ┌────────────────┬──────────────────────────────┐\n");
    printf("    │ Field          │ Value                        │\n");
    printf("    ├────────────────┼──────────────────────────────┤\n");
    printf("    │ L1 Index       │ %-4u (bits[31:22])           │\n", r.l1_index);
    printf("    │ L2 Index       │ %-4u (bits[21:12])           │\n", r.l2_index);
    printf("    │ Offset         │ 0x%03X = %-4u (bits[11:0])   │\n", r.offset, r.offset);
    printf("    │ VPN            │ 0x%05X = %-7u              │\n", r.vpn, r.vpn);
    printf("    └────────────────┴──────────────────────────────┘\n");

    /* ── Step 2: TLB Lookup ── */
    printf("\n  [Step 2] Checking TLB for VPN 0x%05X ...\n", r.vpn);

    if (tlb_lookup(tlb, r.vpn, &r.frame)) {
        /* ─── TLB HIT ─── */
        r.tlb_hit = true;
        printf("  [TLB] *** HIT *** VPN 0x%05X → Frame %u\n",
               r.vpn, r.frame);
    } else {
        /* ─── TLB MISS: walk the page table ─── */
        printf("  [TLB] *** MISS *** — walking page table...\n");

        /* ── Step 3: Page Table Walk ── */
        r.frame = pt_lookup(l1, r.l1_index, r.l2_index);

        if (r.frame == INVALID_FRAME) {
            r.status = TRANS_PAGE_FAULT;
            r.physical_addr = 0;
            return r;
        }

        /* ── Step 4: Install new mapping into TLB ── */
        printf("  [Step 4] Updating TLB: VPN 0x%05X → Frame %u\n",
               r.vpn, r.frame);
        tlb_insert(tlb, r.vpn, r.frame);
    }

    /* ── Step 5: Construct Physical Address ── */
    r.physical_addr = (paddr_t)r.frame * PAGE_SIZE + r.offset;

    printf("\n  [Step 5] Physical Address Calculation:\n");
    printf("           Frame(%u) × PageSize(%u) + Offset(%u)\n",
           r.frame, PAGE_SIZE, r.offset);
    printf("           = %u × %u + %u = %u\n",
           r.frame, PAGE_SIZE, r.offset, r.physical_addr);

    return r;
}

/* ─────────────────────────── vm_print_result ───────────────────────── */
/**
 * vm_print_result — pretty-print a translation result summary.
 */
void vm_print_result(const TranslationResult *r)
{
    printf("\n  ╔══════════════════════════════════════════╗\n");
    printf("  ║          TRANSLATION SUMMARY             ║\n");
    printf("  ╠══════════════════════════════════════════╣\n");
    printf("  ║  Virtual Address :  0x%08X            ║\n", r->virtual_addr);
    printf("  ║  L1 Index        :  %-24u  ║\n", r->l1_index);
    printf("  ║  L2 Index        :  %-24u  ║\n", r->l2_index);
    printf("  ║  Offset          :  0x%03X (%-18u) ║\n", r->offset, r->offset);
    printf("  ║  VPN             :  0x%05X                 ║\n", r->vpn);
    printf("  ║  Frame Number    :  %-24u  ║\n", r->frame);
    printf("  ╠══════════════════════════════════════════╣\n");

    if (r->status == TRANS_PAGE_FAULT) {
        printf("  ║  STATUS          :  *** PAGE FAULT ***    ║\n");
    } else {
        printf("  ║  TLB Result      :  %-24s  ║\n",
               r->tlb_hit ? "HIT" : "MISS (page table walk)");
        printf("  ║  Physical Address:  %-24u  ║\n", r->physical_addr);
        printf("  ║                     (0x%08X)          ║\n", r->physical_addr);
    }
    printf("  ╚══════════════════════════════════════════╝\n");
}

/* ─────────────────────────── vm_run_test_case ──────────────────────── */
/**
 * vm_run_test_case — execute the mandatory lab test case.
 *
 * Virtual address under test: 0x12345678
 *
 * Expected decomposition (correct bit-level parsing):
 *   VPN      = 0x12345678 >> 12 = 0x12345 = 74565
 *   L1 index = 0x12345 >> 10    = 72          [matches lab spec ✓]
 *   L2 index = 0x12345 & 0x3FF  = 837 (0x345) [lab PDF shows 342 — typo]
 *   Offset   = 0x12345678 & FFF = 0x678 = 1656 [matches lab spec ✓]
 *
 * NOTE: The lab document states L2 = 342.  The mathematically correct
 * value per 32-bit binary decomposition is 837 (0x345). The document
 * appears to contain a transcription error. All other values (L1=72,
 * offset=0x678, physical address=206456) are consistent with the correct
 * parsing and frame=50, confirming the frame mapping is authoritative.
 *
 * Mock mapping installed: VPN 0x12345 → Frame 50
 * Expected physical address: 50 × 4096 + 1656 = 206456
 */
void vm_run_test_case(TLB *tlb, L1PageTable *l1)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║              MANDATORY LAB TEST CASE                    ║\n");
    printf("║         Virtual Address: 0x12345678                     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("\n  Setting up mock page table mapping...\n");

    /*
     * Compute the VPN for 0x12345678 to install the mock mapping.
     * VPN = 0x12345678 >> 12 = 0x12345
     */
    vpn_t test_vpn = 0x12345678u >> PAGE_OFFSET_BITS;  /* = 0x12345 */
    frame_t test_frame = 50;

    pt_map(l1, test_vpn, test_frame);

    printf("\n  Running translation for 0x12345678 ...\n");
    TranslationResult r = vm_translate(tlb, l1, 0x12345678u);
    vm_print_result(&r);

    /* ── Verification ── */
    printf("\n  ┌──────────────── Verification ────────────────────────┐\n");
    printf("  │  L1 Index  expected: 72,    got: %-5u  %s  │\n",
           r.l1_index,  r.l1_index  == 72    ? "[PASS]" : "[FAIL]");
    printf("  │  L2 Index  expected: 837,   got: %-5u  %s  │\n",
           r.l2_index,  r.l2_index  == 837   ? "[PASS]" : "[FAIL]");
    printf("  │  Offset    expected: 0x678, got: 0x%03X  %s  │\n",
           r.offset,    r.offset    == 0x678 ? "[PASS]" : "[FAIL]");
    printf("  │  Phys Addr expected: 206456, got: %-6u %s  │\n",
           r.physical_addr, r.physical_addr == 206456 ? "[PASS]" : "[FAIL]");
    printf("  └───────────────────────────────────────────────────────┘\n");
    printf("\n  NOTE: Lab PDF cites L2=342 — correct value is 837 (0x345).\n");
    printf("        Physical address 206456 matches the spec exactly.\n");
}
