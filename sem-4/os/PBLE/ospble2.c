/*
 * PBLE-2 : Simulation of Multi-Level Page Tables with TLB
 *
 * Setup:
 *   - Virtual address  : 32 bits
 *   - Page size        : 4 KB  (2^12 => 12-bit offset)
 *   - Level-1 index    : top 10 bits
 *   - Level-2 index    : next 10 bits
 *   - Offset           : bottom 12 bits
 *   - TLB size         : 16 entries, FIFO replacement
 *
 * VPN = (L1_index, L2_index) — used as TLB key
 * Physical Address = Frame_Number * PAGE_SIZE + Offset
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── constants ──────────────────────────────────────────── */
#define PAGE_SIZE       4096        /* 4 KB */
#define OFFSET_BITS     12
#define L1_BITS         10
#define L2_BITS         10
#define L1_SIZE         (1 << L1_BITS)  /* 1024 */
#define L2_SIZE         (1 << L2_BITS)  /* 1024 */
#define TLB_SIZE        16

/* ── masks & shifts ─────────────────────────────────────── */
#define OFFSET_MASK     ((1U << OFFSET_BITS) - 1)           /* 0x00000FFF */
#define L2_SHIFT        OFFSET_BITS                          /* 12 */
#define L1_SHIFT        (OFFSET_BITS + L2_BITS)              /* 22 */
#define L2_MASK         ((1U << L2_BITS) - 1)
#define L1_MASK         ((1U << L1_BITS) - 1)

/* ── TLB entry ──────────────────────────────────────────── */
typedef struct {
    unsigned int  vpn_l1;
    unsigned int  vpn_l2;
    unsigned int  frame;
    int           valid;
} TLB_Entry;

/* ── globals ────────────────────────────────────────────── */
TLB_Entry      tlb[TLB_SIZE];
int            tlb_head    = 0;
int            tlb_hits    = 0;
int            tlb_misses  = 0;

unsigned int *level1_table[L1_SIZE];

/* ── helper: extract fields from a virtual address ──────── */
void split_va(unsigned int va,
              unsigned int *l1, unsigned int *l2, unsigned int *off)
{
    *off = va  & OFFSET_MASK;
    *l2  = (va >> L2_SHIFT) & L2_MASK;
    *l1  = (va >> L1_SHIFT) & L1_MASK;
}

/* ── TLB lookup: returns frame or -1 on miss ────────────── */
int tlb_lookup(unsigned int l1, unsigned int l2)
{
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].vpn_l1 == l1 && tlb[i].vpn_l2 == l2)
            return (int)tlb[i].frame;
    }
    return -1;
}

/* ── TLB insert (FIFO replacement) ─────────────────────── */
void tlb_insert(unsigned int l1, unsigned int l2, unsigned int frame)
{
    tlb[tlb_head].vpn_l1 = l1;
    tlb[tlb_head].vpn_l2 = l2;
    tlb[tlb_head].frame  = frame;
    tlb[tlb_head].valid  = 1;
    tlb_head = (tlb_head + 1) % TLB_SIZE;
}

/* ── page-table lookup (creates L2 table on demand) ─────── */
unsigned int page_table_lookup(unsigned int l1, unsigned int l2)
{
    static unsigned int next_frame = 10;

    if (level1_table[l1] == NULL) {
        level1_table[l1] = (unsigned int *)calloc(L2_SIZE, sizeof(unsigned int));
        if (!level1_table[l1]) { perror("calloc"); exit(1); }
    }

    if (level1_table[l1][l2] == 0)
        level1_table[l1][l2] = next_frame++;

    return level1_table[l1][l2];
}

/* ── main translation routine ───────────────────────────── */
unsigned long translate(unsigned int va)
{
    unsigned int l1, l2, offset, frame;
    int          hit;

    split_va(va, &l1, &l2, &offset);

    printf("\n--- Translating VA: 0x%08X ---\n", va);
    printf("  L1 index = %u  |  L2 index = %u  |  Offset = 0x%03X (%u)\n",
           l1, l2, offset, offset);

    hit = tlb_lookup(l1, l2);
    if (hit != -1) {
        frame = (unsigned int)hit;
        tlb_hits++;
        printf("  [TLB HIT]  Frame = %u\n", frame);
    } else {
        tlb_misses++;
        printf("  [TLB MISS] Checking page table...\n");

        frame = page_table_lookup(l1, l2);
        printf("  PageTable[%u][%u] -> Frame = %u\n", l1, l2, frame);

        tlb_insert(l1, l2, frame);
        printf("  TLB updated with (VPN: %u,%u) -> Frame %u\n", l1, l2, frame);
    }

    unsigned long pa = (unsigned long)frame * PAGE_SIZE + offset;
    printf("  Physical Address = %u * %u + %u = %lu (0x%lX)\n",
           frame, PAGE_SIZE, offset, pa, pa);

    return pa;
}

/* ── free page table memory ─────────────────────────────── */
void cleanup()
{
    for (int i = 0; i < L1_SIZE; i++)
        if (level1_table[i]) free(level1_table[i]);
}

/* ── entry point ────────────────────────────────────────── */
int main()
{
    memset(tlb, 0, sizeof(tlb));
    memset(level1_table, 0, sizeof(level1_table));

    printf("========================================\n");
    printf(" Multi-Level Page Table + TLB Simulator\n");
    printf("  VA bits=32 | Page=4KB | L1=10b | L2=10b | TLB=%d entries\n", TLB_SIZE);
    printf("========================================\n");

    unsigned int test_va[] = {
        0x12345678,
        0x12345700,
        0xABCD1234,
        0x00001000,
        0xDEADBEEF,
        0x12345678,
    };
    int n = sizeof(test_va) / sizeof(test_va[0]);

    for (int i = 0; i < n; i++)
        translate(test_va[i]);

    printf("\n========== TLB Statistics ==========\n");
    printf("  Total accesses : %d\n", n);
    printf("  TLB Hits       : %d\n", tlb_hits);
    printf("  TLB Misses     : %d\n", tlb_misses);
    printf("  Hit Rate       : %.1f%%\n",
           (n ? (tlb_hits * 100.0 / n) : 0.0));
    printf("=====================================\n");

    cleanup();
    return 0;
}
