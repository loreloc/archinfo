
# Archinfo
Archinfo is a software that gathers informations about your x86 processor using the *cpuid* instruction.  
At the moment it only supports Intel processors.  
  
```
$ bin/archinfo64
Vendor ID: GenuineIntel

Brand: Intel(R) Core(TM) i5-4460  CPU @ 3.20GHz

Stepping ID: 3
Model: C
Family: 6
Extended Model: 3C
Extended Family: 6

Microarchitecture: Haswell - 22 nm

Features:
FPU VME DE PSE TSC MSR PAE MCE CX8 APIC SEP MTRR PGE MCA CMOV PAT PSE-36 CLFSH DS ACPI MMX FXSR SSE SSE2 SS HTT TM PBE SSE3 PCLMULQDQ DTES64 MONITOR DS-CPL VMX EIST TM2 SSSE3 SDBG FMA CMPXCHG16B XTPR PDCM PCID SSE4.1 SSE4.2 X2APIC MOVBE POPCNT TSC-DEADLINE AESNI XSAVE OSXSAVE AVX F16C RDRND

Extended Features:
FSGSBASE IA32-TSC-ADJ BMI1 AVX2 SMEP BMI2 ENHANCED-RMS INVPCID DEPR-FCSDS

Cores: 4
Threads: 4

Core #0
   Cache Level 1 Data
   Cache Level 1 Instruction
   Cache Level 2 Unified
Core #1
   Cache Level 1 Data
   Cache Level 1 Instruction
   Cache Level 2 Unified
Core #2
   Cache Level 1 Data
   Cache Level 1 Instruction
   Cache Level 2 Unified
Core #3
   Cache Level 1 Data
   Cache Level 1 Instruction
   Cache Level 2 Unified

Shared Caches:
   Cache Level 3 Unified

Cache Level 1 Data:
Size: 32 KB, 1 partitions
64 sets, 8-way set associative, 64 byte line size

Cache Level 1 Instruction:
Size: 32 KB, 1 partitions
64 sets, 8-way set associative, 64 byte line size

Cache Level 2 Unified:
Size: 256 KB, 1 partitions
512 sets, 8-way set associative, 64 byte line size

Cache Level 3 Unified:
Size: 6144 KB, 1 partitions
8192 sets, 12-way set associative, 64 byte line size

Cache and TLB informations:
Data TLB: 2 MByte or 4 MByte pages, 4-way set associative, 32 entries and a separate array with 1 GByte pages, 4-way set associative, 4 entries
Data TLB: 4 KByte pages, 4-way set associative, 64 entries
Instruction TLB: 2M/4M pages, fully associative, 8 entries
Instruction TLB: 4KByte pages, 8-way set associative, 128 entries
64-Byte prefetching
Shared 2nd-Level TLB: 4 KByte/2MByte pages, 8-way associative, 1024 entries
```  

