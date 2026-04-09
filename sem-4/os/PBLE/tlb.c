/*
 * tlb.c — Translation Lookaside Buffer (TLB) Implementation
 * ===========================================================
 * Implements a fully-associative TLB with LRU eviction policy.
 *
 * LRU Mechanism:
 *   Each entry stores a logical timestamp (lru_counter).  On every access
 *   (hit or insert) the global clock is incremented and the accessed/new
 *   entry receives the new clock value.  Eviction always targets the entry
 *   whose lru_counter is smallest (i.e., least recently used).
 */

#include <stdio.h>
#include <string.h>
#include "vm.h"

/* ─────────────────────────── tlb_init ──────────────────────────────── */
/**
 * tlb_init — zero-initialise all TLB entries and counters.
 * Must be called before any other TLB operation.
 */
void tlb_init(TLB *tlb)
{
    memset(tlb, 0, sizeof(TLB));
    /* All entries start as invalid; no explicit loop needed after memset */
}

/* ─────────────────────────── tlb_lookup ────────────────────────────── */
/**
 * tlb_lookup — probe the TLB for a given VPN.
 *
 * @param tlb       pointer to TLB
 * @param vpn       virtual page number to look up
 * @param out_frame output: frame number if hit
 * @return true on TLB HIT, false on TLB MISS
 *
 * On a HIT the entry's LRU counter is refreshed (touch).
 */
bool tlb_lookup(TLB *tlb, vpn_t vpn, frame_t *out_frame)
{
    tlb->clock++;   /* advance logical clock for every access */

    for (int i = 0; i < TLB_SIZE; i++) {
        TLBEntry *e = &tlb->entries[i];
        if (e->valid && e->vpn == vpn) {
            /* ── HIT ── refresh this entry's recency timestamp */
            e->lru_counter = tlb->clock;
            *out_frame = e->frame;
            tlb->hits++;
            return true;
        }
    }

    tlb->misses++;
    return false;  /* TLB MISS */
}

/* ─────────────────────────── tlb_insert ────────────────────────────── */
/**
 * tlb_insert — add a new VPN→frame mapping to the TLB.
 *
 * Steps:
 *  1. If an empty (invalid) slot exists, use it.
 *  2. Otherwise, evict the LRU entry (lowest lru_counter among valid entries).
 *
 * @param tlb   pointer to TLB
 * @param vpn   virtual page number to install
 * @param frame corresponding physical frame number
 */
void tlb_insert(TLB *tlb, vpn_t vpn, frame_t frame)
{
    tlb->clock++;

    /* ── Pass 1: find an empty slot ── */
    for (int i = 0; i < TLB_SIZE; i++) {
        if (!tlb->entries[i].valid) {
            tlb->entries[i].valid       = true;
            tlb->entries[i].vpn         = vpn;
            tlb->entries[i].frame       = frame;
            tlb->entries[i].lru_counter = tlb->clock;
            return;
        }
    }

    /* ── Pass 2: LRU eviction (no empty slots) ── */
    int lru_idx = 0;
    uint64_t min_counter = tlb->entries[0].lru_counter;

    for (int i = 1; i < TLB_SIZE; i++) {
        if (tlb->entries[i].lru_counter < min_counter) {
            min_counter = tlb->entries[i].lru_counter;
            lru_idx = i;
        }
    }

    tlb->evictions++;
    printf("  [TLB] Evicting VPN 0x%05X (LRU) → installing VPN 0x%05X\n",
           tlb->entries[lru_idx].vpn, vpn);

    tlb->entries[lru_idx].vpn         = vpn;
    tlb->entries[lru_idx].frame       = frame;
    tlb->entries[lru_idx].lru_counter = tlb->clock;
    /* valid stays true */
}

/* ─────────────────────────── tlb_flush ─────────────────────────────── */
/**
 * tlb_flush — invalidate all TLB entries (e.g., on context switch).
 * Does NOT reset statistics so cumulative counts are preserved.
 */
void tlb_flush(TLB *tlb)
{
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb->entries[i].valid = false;
    }
    printf("  [TLB] Flushed all %d entries.\n", TLB_SIZE);
}

/* ─────────────────────────── tlb_print_stats ───────────────────────── */
/**
 * tlb_print_stats — print cumulative hit/miss/eviction statistics.
 */
void tlb_print_stats(const TLB *tlb)
{
    uint32_t total = tlb->hits + tlb->misses;
    float    hit_rate = (total > 0) ? (100.0f * tlb->hits / total) : 0.0f;

    printf("\n┌─────────────────────────────┐\n");
    printf("│        TLB Statistics       │\n");
    printf("├──────────────┬──────────────┤\n");
    printf("│ Accesses     │ %-12u │\n", total);
    printf("│ Hits         │ %-12u │\n", tlb->hits);
    printf("│ Misses       │ %-12u │\n", tlb->misses);
    printf("│ Hit Rate     │ %-11.2f%% │\n", hit_rate);
    printf("│ Evictions    │ %-12u │\n", tlb->evictions);
    printf("└──────────────┴──────────────┘\n");
}

/* ─────────────────────────── tlb_dump ──────────────────────────────── */
/**
 * tlb_dump — print all valid TLB entries (for debugging).
 */
void tlb_dump(const TLB *tlb)
{
    printf("\n  ╔══════ TLB Contents (capacity=%d) ══════╗\n", TLB_SIZE);
    printf("  ║  Slot │    VPN    │  Frame  │ LRU tick ║\n");
    printf("  ╠═══════╪═══════════╪═════════╪══════════╣\n");

    bool any_valid = false;
    for (int i = 0; i < TLB_SIZE; i++) {
        const TLBEntry *e = &tlb->entries[i];
        if (e->valid) {
            any_valid = true;
            printf("  ║  %4d │ 0x%07X │ %7u │ %8llu  ║\n",
                   i, e->vpn, e->frame,
                   (unsigned long long)e->lru_counter);
        }
    }
    if (!any_valid)
        printf("  ║               (empty)                   ║\n");

    printf("  ╚═══════╧═══════════╧═════════╧══════════╝\n");
}
