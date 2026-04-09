/*
 * vm.h — Virtual Memory Simulation: Shared Definitions & API
 * =============================================================
 * Architecture: 32-bit virtual address space
 * Page size   : 4 KB  (2^12 bytes  → 12-bit offset)
 * Page table  : 2-level (10-bit L1 index + 10-bit L2 index)
 *
 * Virtual Address Layout:
 *  31          22 21          12 11            0
 *  ┌─────────────┬─────────────┬───────────────┐
 *  │  L1 (10b)   │  L2 (10b)   │  Offset (12b) │
 *  └─────────────┴─────────────┴───────────────┘
 *
 * Physical Address = Frame Number × PAGE_SIZE + Offset
 */

#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <stdbool.h>

/* ─────────────────────── Architecture Constants ─────────────────────── */

#define PAGE_SIZE          4096u          /* 4 KB                          */
#define PAGE_OFFSET_BITS   12             /* bits[11:0]                    */
#define L1_INDEX_BITS      10             /* bits[31:22]                   */
#define L2_INDEX_BITS      10             /* bits[21:12]                   */

#define OFFSET_MASK        ((1u << PAGE_OFFSET_BITS) - 1)   /* 0x00000FFF */
#define L2_INDEX_MASK      ((1u << L2_INDEX_BITS)   - 1)   /* 0x000003FF */
#define L1_INDEX_MASK      ((1u << L1_INDEX_BITS)   - 1)   /* 0x000003FF */

/* Number of entries in each level of the page table */
#define L1_TABLE_ENTRIES   (1u << L1_INDEX_BITS)   /* 1024 */
#define L2_TABLE_ENTRIES   (1u << L2_INDEX_BITS)   /* 1024 */

/* TLB capacity — adjustable; must be ≥ 1 */
#define TLB_SIZE           16

/* Sentinel value: an L2 table pointer / frame number that is not mapped */
#define INVALID_FRAME      UINT32_MAX
#define INVALID_L2_BASE    NULL

/* ──────────────────────── Type Aliases ──────────────────────────────── */

typedef uint32_t  vaddr_t;    /* 32-bit virtual address  */
typedef uint32_t  paddr_t;    /* 32-bit physical address */
typedef uint32_t  vpn_t;      /* virtual page number (top 20 bits) */
typedef uint32_t  frame_t;    /* physical frame number              */

/* ──────────────────────── Page Table Structures ─────────────────────── */

/*
 * L2PageTable — one L2 table holds 1024 frame mappings.
 * frame[i] == INVALID_FRAME means the page is not present (page fault).
 */
typedef struct {
    frame_t frame[L2_TABLE_ENTRIES];
} L2PageTable;

/*
 * L1PageTable — the top-level table; each entry either points to an
 * L2PageTable or is NULL (meaning no L2 table allocated yet).
 */
typedef struct {
    L2PageTable *entry[L1_TABLE_ENTRIES];
} L1PageTable;

/* ──────────────────────── TLB Structures ────────────────────────────── */

/*
 * TLBEntry — one slot in the TLB.
 *   valid : whether this slot holds a live mapping
 *   vpn   : virtual page number cached
 *   frame : corresponding physical frame
 *   lru_counter: logical clock value for LRU eviction
 */
typedef struct {
    bool    valid;
    vpn_t   vpn;
    frame_t frame;
    uint64_t lru_counter;  /* higher = more recently used */
} TLBEntry;

/*
 * TLB — the full TLB structure with a logical clock for LRU tracking.
 */
typedef struct {
    TLBEntry entries[TLB_SIZE];
    uint64_t clock;     /* incremented on every access (hit or miss)  */
    uint32_t hits;      /* cumulative hit count                        */
    uint32_t misses;    /* cumulative miss count                       */
    uint32_t evictions; /* cumulative eviction count                   */
} TLB;

/* ──────────────────────── Translation Result ────────────────────────── */

typedef enum {
    TRANS_OK        = 0,   /* successful translation                  */
    TRANS_PAGE_FAULT = 1,  /* L1 or L2 entry missing → page fault     */
} TranslationStatus;

/*
 * TranslationResult — full breakdown of one address translation.
 */
typedef struct {
    /* Input decomposition */
    vaddr_t  virtual_addr;
    uint32_t l1_index;
    uint32_t l2_index;
    uint32_t offset;
    vpn_t    vpn;
    /* Translation outcome */
    frame_t  frame;
    paddr_t  physical_addr;
    bool     tlb_hit;
    TranslationStatus status;
} TranslationResult;

/* ──────────────────────── Function Declarations ─────────────────────── */

/* --- TLB API (tlb.c) --- */
void    tlb_init(TLB *tlb);
bool    tlb_lookup(TLB *tlb, vpn_t vpn, frame_t *out_frame);
void    tlb_insert(TLB *tlb, vpn_t vpn, frame_t frame);
void    tlb_flush(TLB *tlb);
void    tlb_print_stats(const TLB *tlb);
void    tlb_dump(const TLB *tlb);

/* --- Page Table API (pagetable.c) --- */
L1PageTable *pt_create(void);
void         pt_destroy(L1PageTable *l1);
bool         pt_map(L1PageTable *l1, vpn_t vpn, frame_t frame);
frame_t      pt_lookup(const L1PageTable *l1, uint32_t l1_idx, uint32_t l2_idx);

/* --- Translation Engine API (vm.c) --- */
void vm_init(TLB *tlb, L1PageTable *l1);
TranslationResult vm_translate(TLB *tlb, L1PageTable *l1, vaddr_t vaddr);
void vm_print_result(const TranslationResult *r);
void vm_run_test_case(TLB *tlb, L1PageTable *l1);

/* --- Utility --- */
void decompose_address(vaddr_t addr,
                       uint32_t *l1_idx,
                       uint32_t *l2_idx,
                       uint32_t *offset,
                       vpn_t    *vpn);

#endif /* VM_H */
