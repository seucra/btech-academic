/*
 * main.c — CLI Entry Point for the Virtual Memory Simulation
 * ============================================================
 * Provides an interactive REPL-style interface and a batch test suite.
 *
 * Usage:
 *   ./vm_sim                    — run mandatory test case + interactive mode
 *   ./vm_sim --test             — run mandatory test case only, then exit
 *   ./vm_sim --batch            — run built-in multi-address stress test
 *   ./vm_sim --help             — print usage information
 *
 * In interactive mode the user can type hexadecimal virtual addresses
 * (with or without the 0x prefix).  Commands:
 *   quit / exit / q             — exit the simulator
 *   stats                       — print TLB statistics
 *   tlb                         — dump all valid TLB entries
 *   flush                       — flush the TLB
 *   map <hex_vpn> <frame>       — manually install a page-table mapping
 *   help                        — print command list
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "vm.h"

/* ─────────────────────────── Forward Declarations ───────────────────── */

static void  print_banner(void);
static void  print_help(void);
static void  run_batch_tests(TLB *tlb, L1PageTable *l1);
static void  repl(TLB *tlb, L1PageTable *l1);
static bool  parse_hex(const char *s, uint32_t *out);

/* ─────────────────────────────── main ───────────────────────────────── */

int main(int argc, char *argv[])
{
    print_banner();

    /* ── Parse CLI flags ── */
    bool flag_test  = false;
    bool flag_batch = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--test")  == 0) flag_test  = true;
        else if (strcmp(argv[i], "--batch") == 0) flag_batch = true;
        else if (strcmp(argv[i], "--help")  == 0) { print_help(); return 0; }
        else {
            fprintf(stderr, "[ERROR] Unknown flag: %s  (try --help)\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    /* ── Initialise subsystems ── */
    TLB          tlb;
    L1PageTable *l1 = pt_create();
    vm_init(&tlb, l1);

    /* ── Always run the mandatory test case ── */
    vm_run_test_case(&tlb, l1);
    tlb_print_stats(&tlb);

    if (flag_test) {
        /* Test-only mode: print result and exit */
        pt_destroy(l1);
        return EXIT_SUCCESS;
    }

    if (flag_batch) {
        /* Flush TLB between modes for a clean slate */
        tlb_flush(&tlb);
        run_batch_tests(&tlb, l1);
        tlb_print_stats(&tlb);
        pt_destroy(l1);
        return EXIT_SUCCESS;
    }

    /* ── Default: drop into interactive REPL ── */
    tlb_flush(&tlb);
    repl(&tlb, l1);

    /* ── Cleanup ── */
    pt_destroy(l1);
    return EXIT_SUCCESS;
}

/* ─────────────────────────── print_banner ───────────────────────────── */

static void print_banner(void)
{
    printf("\n");
    printf("  ╔══════════════════════════════════════════════════════════╗\n");
    printf("  ║        VIRTUAL MEMORY TRANSLATION SIMULATOR             ║\n");
    printf("  ║   32-bit Addr | 2-Level Page Table | TLB (LRU, sz=%2d)  ║\n", TLB_SIZE);
    printf("  ║   Page Size: 4 KB  |  L1: 10 bits  |  L2: 10 bits      ║\n");
    printf("  ╚══════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/* ─────────────────────────── print_help ─────────────────────────────── */

static void print_help(void)
{
    printf("\nUSAGE:\n");
    printf("  ./vm_sim              Interactive mode (default)\n");
    printf("  ./vm_sim --test       Run mandatory test case only\n");
    printf("  ./vm_sim --batch      Run batch stress test suite\n");
    printf("  ./vm_sim --help       Show this help message\n");
    printf("\nINTERACTIVE COMMANDS:\n");
    printf("  <hex_addr>            Translate a virtual address (e.g. 0xDEADBEEF)\n");
    printf("  map <hex_vpn> <dec_frame>  Install a page table entry manually\n");
    printf("  stats                 Print TLB hit/miss statistics\n");
    printf("  tlb                   Dump current TLB contents\n");
    printf("  flush                 Invalidate all TLB entries\n");
    printf("  help                  Show this help message\n");
    printf("  quit / exit / q       Exit the simulator\n\n");
}

/* ─────────────────────────── run_batch_tests ────────────────────────── */
/**
 * run_batch_tests — translate a set of pre-mapped virtual addresses to
 * demonstrate TLB hits, misses, and the LRU eviction policy.
 *
 * Addresses chosen to exercise:
 *   - Cold TLB miss on first access to each unique VPN
 *   - Warm TLB hit when the same VPN is re-accessed
 *   - LRU eviction once TLB_SIZE unique VPNs have been loaded
 */
static void run_batch_tests(TLB *tlb, L1PageTable *l1)
{
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                 BATCH STRESS TEST SUITE                 ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");

    /* Install mappings for a range of VPNs: VPN i → Frame (100+i) */
    printf("\n  Installing %d page table mappings...\n", TLB_SIZE + 4);
    for (uint32_t i = 0; i < (uint32_t)(TLB_SIZE + 4); i++) {
        /* Spread VPNs across different L1 buckets for realism */
        vpn_t vpn = (i * 128) & 0xFFFFF;   /* stride 128 pages = 512 KB apart */
        pt_map(l1, vpn, 100 + i);
    }

    /* Build corresponding virtual addresses: VPN << 12 | some offset */
    struct { vaddr_t addr; const char *label; } tests[] = {
        { 0x00000100u, "First  access (cold miss, VPN=0x00000)" },
        { 0x00000200u, "Second access (TLB HIT,  VPN=0x00000)" },
        { 0x00080100u, "VPN=0x00080 (cold miss)"               },
        { 0x00100100u, "VPN=0x00100 (cold miss)"               },
        { 0x00180100u, "VPN=0x00180 (cold miss)"               },
        { 0x00200100u, "VPN=0x00200 (cold miss)"               },
        { 0x00280100u, "VPN=0x00280 (cold miss)"               },
        { 0x00300100u, "VPN=0x00300 (cold miss)"               },
        { 0x00380100u, "VPN=0x00380 (cold miss)"               },
        { 0x00400100u, "VPN=0x00400 (cold miss)"               },
        { 0x00480100u, "VPN=0x00480 (cold miss)"               },
        { 0x00500100u, "VPN=0x00500 (cold miss)"               },
        { 0x00580100u, "VPN=0x00580 (cold miss)"               },
        { 0x00600100u, "VPN=0x00600 (cold miss)"               },
        { 0x00680100u, "VPN=0x00680 (cold miss)"               },
        { 0x00700100u, "VPN=0x00700 (cold miss)"               },
        /* TLB now full; next miss should trigger LRU eviction */
        { 0x00780100u, "VPN=0x00780 (LRU eviction expected)"   },
        { 0x00800100u, "VPN=0x00800 (LRU eviction expected)"   },
        /* Re-access the very first entry — likely evicted, will miss again */
        { 0x00000100u, "VPN=0x00000 re-access (may be evicted)"},
    };

    int n = (int)(sizeof(tests) / sizeof(tests[0]));
    for (int i = 0; i < n; i++) {
        printf("\n[Test %2d/%d] %s\n", i + 1, n, tests[i].label);
        TranslationResult r = vm_translate(tlb, l1, tests[i].addr);
        vm_print_result(&r);
    }

    tlb_dump(tlb);
}

/* ─────────────────────────── repl ──────────────────────────────────── */
/**
 * repl — interactive read-eval-print loop.
 */
static void repl(TLB *tlb, L1PageTable *l1)
{
    char line[256];

    printf("\n  Entering interactive mode. Type 'help' for commands.\n");
    printf("  Type hex addresses (e.g. 0xDEAD0000) to translate them.\n\n");

    while (1) {
        printf("vm_sim> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            /* EOF (Ctrl-D) */
            printf("\n");
            break;
        }

        /* Strip trailing newline */
        line[strcspn(line, "\n")] = '\0';

        /* Skip blank lines */
        if (line[0] == '\0') continue;

        /* ── Command dispatch ── */
        if (strcmp(line, "quit") == 0 ||
            strcmp(line, "exit") == 0 ||
            strcmp(line, "q")    == 0) {
            printf("  Goodbye.\n");
            break;
        }

        if (strcmp(line, "help") == 0) {
            print_help();
            continue;
        }

        if (strcmp(line, "stats") == 0) {
            tlb_print_stats(tlb);
            continue;
        }

        if (strcmp(line, "tlb") == 0) {
            tlb_dump(tlb);
            continue;
        }

        if (strcmp(line, "flush") == 0) {
            tlb_flush(tlb);
            continue;
        }

        /* map <hex_vpn> <frame> */
        if (strncmp(line, "map ", 4) == 0) {
            char vpn_str[64], frame_str[64];
            if (sscanf(line + 4, "%63s %63s", vpn_str, frame_str) == 2) {
                uint32_t vpn_val, frame_val;
                if (parse_hex(vpn_str, &vpn_val) && parse_hex(frame_str, &frame_val)) {
                    pt_map(l1, (vpn_t)vpn_val, (frame_t)frame_val);
                } else {
                    printf("  [ERROR] Usage: map <hex_vpn> <dec_or_hex_frame>\n");
                }
            } else {
                printf("  [ERROR] Usage: map <hex_vpn> <frame_number>\n");
            }
            continue;
        }

        /* Try to interpret the input as a hex virtual address */
        uint32_t vaddr;
        if (parse_hex(line, &vaddr)) {
            TranslationResult r = vm_translate(tlb, l1, (vaddr_t)vaddr);
            vm_print_result(&r);
        } else {
            printf("  [ERROR] Unknown command or invalid address: '%s'\n", line);
            printf("          Type 'help' for a list of commands.\n");
        }
    }
}

/* ─────────────────────────── parse_hex ──────────────────────────────── */
/**
 * parse_hex — parse a string into a uint32_t, accepting both 0x prefix
 * and plain decimal notation.
 *
 * @param s   input string
 * @param out output uint32_t
 * @return true on success, false on parse failure
 */
static bool parse_hex(const char *s, uint32_t *out)
{
    if (!s || *s == '\0') return false;

    char *end;
    errno = 0;

    /* strtoul handles "0x..." and plain decimal */
    unsigned long val = strtoul(s, &end, 0);

    /* Must consume the whole string and not overflow */
    if (errno != 0 || end == s || *end != '\0' || val > UINT32_MAX) {
        return false;
    }

    *out = (uint32_t)val;
    return true;
}
