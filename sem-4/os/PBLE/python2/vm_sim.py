"""
Virtual Memory Translation Simulation (Simplified)
===================================================
System Specifications:
- Architecture: 32-bit virtual addresses
- Page Size   : 4 KB (12-bit offset)
- Page Table  : 2-level (10-bit L1, 10-bit L2)
- Address Format: | 10 bits (L1) | 10 bits (L2) | 12 bits (Offset) |
"""

import sys

# Constants
PAGE_SIZE = 4096
OFFSET_BITS = 12
INDEX_BITS = 10
TLB_CAPACITY = 16

class TLB:
    """Translation Lookaside Buffer with FIFO replacement policy."""
    def __init__(self, size=TLB_CAPACITY):
        self.size = size
        self.cache = []  # List of [vpn, frame_number]
        self.hits = 0
        self.misses = 0

    def lookup(self, vpn):
        """Check if VPN is in TLB."""
        for entry in self.cache:
            if entry[0] == vpn:
                self.hits += 1
                return entry[1]
        self.misses += 1
        return None

    def update(self, vpn, frame):
        """Add mapping to TLB, using FIFO replacement if full."""
        # Check if already exists (shouldn't happen on miss but let's be safe)
        for entry in self.cache:
            if entry[0] == vpn:
                entry[1] = frame
                return
        
        if len(self.cache) >= self.size:
            self.cache.pop(0)  # Remove oldest (FIFO)
        self.cache.append([vpn, frame])

class PageTable:
    """Two-level page table implemented using nested dictionaries."""
    def __init__(self):
        # {l1_index: {l2_index: frame_number}}
        self.table = {}

    def map(self, l1, l2, frame):
        """Create a mapping for given L1/L2 indices."""
        if l1 not in self.table:
            self.table[l1] = {}
        self.table[l1][l2] = frame

    def lookup(self, l1, l2):
        """Retrieve frame number or None if not mapped."""
        if l1 in self.table and l2 in self.table[l1]:
            return self.table[l1][l2]
        return None

def translate(va, tlb, pt):
    """Perform full address translation."""
    print(f"\n--- Translating: 0x{va:08X} ---")

    # 1. Parse virtual address
    offset = va & 0xFFF              # Last 12 bits
    vpn = va >> OFFSET_BITS          # Top 20 bits
    l2_index = vpn & 0x3FF           # Next 10 bits
    l1_index = vpn >> INDEX_BITS     # First 10 bits

    print(f"L1 Index: {l1_index}")
    print(f"L2 Index: {l2_index}")
    print(f"Offset  : 0x{offset:03X}")

    # 2. Check TLB
    print("Checking TLB...", end=" ")
    frame = tlb.lookup(vpn)
    
    if frame is not None:
        print(f"HIT! Frame: {frame}")
    else:
        print("MISS. Consulting Page Table...", end=" ")
        # 3. Consult Page Table
        frame = pt.lookup(l1_index, l2_index)
        if frame is not None:
            print(f"Found! Frame: {frame}")
            # 4. Update TLB
            tlb.update(vpn, frame)
        else:
            print("ERROR: Page Fault (Mapping not found).")
            return None

    # 5. Construct Physical Address
    pa = frame * PAGE_SIZE + offset
    print(f"Physical Address: {frame} * 4096 + {offset} = {pa}")
    return pa

def run_lab_test():
    """Mandatory test case from OS_Lab_PBLE2: 0x12345678."""
    print("========================================")
    print("RUNNING MANDATORY LAB TEST CASE")
    print("Address: 0x12345678")
    print("========================================")
    
    tlb = TLB()
    pt = PageTable()
    
    # Per requirements: parse L1=72, L2=837 (Math) / 342 (Spec), Offset=0x678
    # Mapping L1=72, L2=837 to frame 50
    # Note: Binary structure of 0x12345678 is 0001001000 | 1101000101 | 011001111000
    # L1 = 72, L2 = 837. We map the mathematical indices.
    pt.map(72, 837, 50)
    
    pa = translate(0x12345678, tlb, pt)
    
    if pa == 206456:
        print("\nRESULT: SUCCESS! PA matches expected 206456.")
    else:
        print(f"\nRESULT: FAILED. Got {pa}, expected 206456.")

def interactive_mode():
    """Simple command-line interface for manual inputs."""
    tlb = TLB()
    pt = PageTable()
    
    # Pre-mapping some data
    pt.map(72, 837, 50)  # The Lab test case
    pt.map(0, 0, 100)    # Mapping 0x00000000 to frame 100
    
    print("\n--- Interactive Simulation ---")
    print("Commands: <hex_address>, 'stats', 'exit'")
    while True:
        try:
            cmd = input("\nEnter VA (hex) or command: ").strip().lower()
            if cmd == 'exit': break
            if cmd == 'stats':
                print(f"TLB Hits: {tlb.hits}, Misses: {tlb.misses}")
                continue
            
            # Remove 0x prefix if present
            va = int(cmd, 16)
            translate(va, tlb, pt)
        except ValueError:
            print("Invalid input. Please enter a hex address.")
        except KeyboardInterrupt:
            break

if __name__ == "__main__":
    run_lab_test()
    interactive_mode()
