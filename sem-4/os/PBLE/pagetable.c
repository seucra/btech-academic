/*
 * pagetable.c — Two-Level Page Table Implementation
 * ===================================================
 * Provides dynamic allocation and management of a 2-level page table:
 *
 *   L1 Table  (1024 entries, statically allocated)
 *     └── L2 Table  (1024 entries per L2, dynamically allocated on demand)
 *           └── Physical Frame Number  (or INVALID_FRAME if not mapped)
 *
 * Memory is allocated lazily: an L2 table is only created when the first
 * page that hashes into its L1 slot is mapped.  This is realistic — a
 * real OS kernel does the same thing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"

/* ─────────────────────────── pt_create ─────────────────────────────── */
/**
 * pt_create — allocate and initialise an empty two-level page table.
 *
 * All L1 entries are set to NULL (no L2 table allocated yet).
 *
 * @return pointer to a newly allocated L1PageTable, or exits on OOM.
 */
L1PageTable *pt_create(void)
{
    L1PageTable *l1 = (L1PageTable *)malloc(sizeof(L1PageTable));
    if (!l1) {
        fprintf(stderr, "[FATAL] Out of memory allocating L1 page table.\n");
        exit(EXIT_FAILURE);
    }

    /* Null-initialise all L1 entries — no L2 tables exist yet */
    memset(l1->entry, 0, sizeof(l1->entry));
    return l1;
}

/* ─────────────────────────── pt_destroy ────────────────────────────── */
/**
 * pt_destroy — free all dynamically allocated L2 tables and the L1 itself.
 * Must be called before program exit to avoid memory leaks.
 */
void pt_destroy(L1PageTable *l1)
{
    if (!l1) return;
    for (int i = 0; i < (int)L1_TABLE_ENTRIES; i++) {
        if (l1->entry[i]) {
            free(l1->entry[i]);
            l1->entry[i] = NULL;
        }
    }
    free(l1);
}

/* ─────────────────────────── pt_map ─────────────────────────────────── */
/**
 * pt_map — install a VPN → frame mapping in the two-level page table.
 *
 *  1. Compute L1 and L2 indices from the VPN.
 *  2. If the L2 table for this L1 entry doesn't exist yet, allocate it.
 *  3. Write the frame number into the appropriate L2 slot.
 *
 * @param l1    root-level page table
 * @param vpn   virtual page number (top 20 bits of vaddr)
 * @param frame physical frame number to map to
 * @return true on success, false on allocation failure
 */
bool pt_map(L1PageTable *l1, vpn_t vpn, frame_t frame)
{
    uint32_t l1_idx = (vpn >> L2_INDEX_BITS) & L1_INDEX_MASK;
    uint32_t l2_idx = vpn & L2_INDEX_MASK;

    /* Allocate L2 table lazily */
    if (!l1->entry[l1_idx]) {
        L2PageTable *l2 = (L2PageTable *)malloc(sizeof(L2PageTable));
        if (!l2) {
            fprintf(stderr, "[ERROR] Out of memory allocating L2 table for "
                            "L1[%u].\n", l1_idx);
            return false;
        }
        /* Mark all entries in the new L2 table as 'not present' */
        for (int i = 0; i < (int)L2_TABLE_ENTRIES; i++)
            l2->frame[i] = INVALID_FRAME;

        l1->entry[l1_idx] = l2;
        printf("  [PT] Allocated new L2 table for L1[%u]\n", l1_idx);
    }

    l1->entry[l1_idx]->frame[l2_idx] = frame;
    printf("  [PT] Mapped VPN 0x%05X (L1[%u][%u]) → Frame %u\n",
           vpn, l1_idx, l2_idx, frame);
    return true;
}

/* ─────────────────────────── pt_lookup ─────────────────────────────── */
/**
 * pt_lookup — walk the two-level page table to find a frame number.
 *
 *  1. Look up L1[l1_idx]; if NULL → page fault (no L2 table exists).
 *  2. Look up L2[l2_idx]; if INVALID_FRAME → page fault (page not mapped).
 *  3. Otherwise return the frame number.
 *
 * @param l1      root page table
 * @param l1_idx  bits[31:22] of the virtual address
 * @param l2_idx  bits[21:12] of the virtual address
 * @return frame number, or INVALID_FRAME on page fault
 */
frame_t pt_lookup(const L1PageTable *l1, uint32_t l1_idx, uint32_t l2_idx)
{
    /* Step 1 — L1 lookup */
    if (!l1->entry[l1_idx]) {
        printf("  [PT] L1[%u] is NULL → no L2 table allocated → PAGE FAULT\n",
               l1_idx);
        return INVALID_FRAME;
    }

    /* Step 2 — L2 lookup */
    frame_t frame = l1->entry[l1_idx]->frame[l2_idx];
    if (frame == INVALID_FRAME) {
        printf("  [PT] L1[%u][%u] = INVALID → page not mapped → PAGE FAULT\n",
               l1_idx, l2_idx);
        return INVALID_FRAME;
    }

    printf("  [PT] L1[%u] → L2 table @ %p\n",
           l1_idx, (void *)l1->entry[l1_idx]);
    printf("  [PT] L2[%u] → Frame %u\n", l2_idx, frame);
    return frame;
}
