
/*
	archinfo - Copyright (c) 2017 loreloc - lorenzoloconte@outlook.it

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgement in the product documentation would be
	appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <stdint.h>

#define CPUID(leaf, a, b, c, d) \
	__asm__ __volatile__ ("cpuid\n\t" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(leaf))

#define CPUID_EXT(leaf, subleaf, a, b, c, d) \
	__asm__ __volatile__ ("cpuid\n\t" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(leaf), "c"(subleaf))

#define EDX_FEATURES_SIZE     29
#define ECX_FEATURES_SIZE     30
#define EBX_EXT_FEATURES_SIZE 30
#define ECX_EXT_FEATURES_SIZE  6

// cpu vendor id
typedef union
{
	struct
	{
		uint32_t dword0;
		uint32_t dword1;
		uint32_t dword2;
	};

	uint8_t id[13];

} cpu_vendor_t;

// cpu signature
typedef union
{
	struct
	{
		unsigned stepping   : 4;
		unsigned model      : 4;
		unsigned family     : 4;
		unsigned type       : 2;
		unsigned _reserved0 : 2;
		unsigned model_ext  : 4;
		unsigned family_ext : 8;
		unsigned _reserved1 : 4;
	};

	uint32_t value;

} cpu_signature_t;

// cpu cache
typedef struct
{
	uint32_t level;
	uint32_t size;
	const char* type;
	uint32_t mask;

	union
	{
		struct
		{
			unsigned line_size  : 12;
			unsigned partitions : 10;
			unsigned ways       : 10;
			uint32_t sets;
		};

		struct
		{
			uint32_t params_lo;
			uint32_t params_hi;
		};

		uint64_t params;
	};

} cpu_cache_t;

// cpu features
typedef struct
{
	union
	{
		struct
		{
			unsigned fpu        : 1;
			unsigned vme        : 1;
			unsigned de         : 1;
			unsigned pse        : 1;
			unsigned tsc        : 1;
			unsigned msr        : 1;
			unsigned pae        : 1;
			unsigned mce        : 1;
			unsigned cx8        : 1;
			unsigned apic       : 1;
			unsigned _res0      : 1;
			unsigned sep        : 1;
			unsigned mtrr       : 1;
			unsigned pge        : 1;
			unsigned mca        : 1;
			unsigned cmov       : 1;
			unsigned pat        : 1;
			unsigned pse_36     : 1;
			unsigned psn        : 1;
			unsigned clfsh      : 1;
			unsigned _res1      : 1;
			unsigned ds         : 1;
			unsigned acpi       : 1;
			unsigned mmx        : 1;
			unsigned fxsr       : 1;
			unsigned sse        : 1;
			unsigned sse2       : 1;
			unsigned ss         : 1;
			unsigned htt        : 1;
			unsigned tm         : 1;
			unsigned _res2      : 1;
			unsigned pbe        : 1;
		};

		uint32_t value;

	} edx;

	union
	{
		struct
		{
			unsigned sse3       : 1;
			unsigned pclmulqdq  : 1;
			unsigned dtes64     : 1;
			unsigned monitor    : 1;
			unsigned ds_cpl     : 1;
			unsigned vmx        : 1;
			unsigned smx        : 1;
			unsigned eist       : 1;
			unsigned tm2        : 1;
			unsigned ssse3      : 1;
			unsigned cnxt_id    : 1;
			unsigned sdbg       : 1;
			unsigned fma        : 1;
			unsigned cmpxchg16b : 1;
			unsigned xtpr       : 1;
			unsigned pdcm       : 1;
			unsigned _res0      : 1;
			unsigned pcid       : 1;
			unsigned dca        : 1;
			unsigned sse4_1     : 1;
			unsigned sse4_2     : 1;
			unsigned x2apic     : 1;
			unsigned movbe      : 1;
			unsigned popcnt     : 1;
			unsigned tsc_dline  : 1;
			unsigned aesni      : 1;
			unsigned xsave      : 1;
			unsigned osxsave    : 1;
			unsigned avx        : 1;
			unsigned f16c       : 1;
			unsigned rdrand     : 1;
			unsigned _res1      : 1;
		};

		uint32_t value;

	} ecx;

} cpu_features_t;

// cpu extended features
typedef struct
{
	union
	{
		struct
		{
			unsigned fsgsbase     : 1;
			unsigned ia32_tsc_adj : 1;
			unsigned sgx          : 1;
			unsigned bmi1         : 1;
			unsigned hle          : 1;
			unsigned avx2         : 1;
			unsigned fdp_excp     : 1;
			unsigned smep         : 1;
			unsigned bmi2         : 1;
			unsigned enhanced_rms : 1;
			unsigned invpcid      : 1;
			unsigned rtm          : 1;
			unsigned rdt_m        : 1;
			unsigned depr_fcsds   : 1;
			unsigned mpx          : 1;
			unsigned rdt_a        : 1;
			unsigned avx512f      : 1;
			unsigned avx512dq     : 1;
			unsigned rdseed       : 1;
			unsigned adx          : 1;
			unsigned smap         : 1;
			unsigned _res0        : 1;
			unsigned _res1        : 1;
			unsigned clflushopt   : 1;
			unsigned clwb         : 1;
			unsigned intel_ptrace : 1;
			unsigned avx512pf     : 1;
			unsigned avx512er     : 1;
			unsigned avx512cd     : 1;
			unsigned sha          : 1;
			unsigned avx512bw     : 1;
			unsigned avx512vl     : 1;
		};

		uint32_t value;

	} ebx;

	union
	{
		struct
		{
			unsigned prefetchwt1  : 1;
			unsigned _res0        : 1;
			unsigned umip         : 1;
			unsigned pku          : 1;
			unsigned ospke        : 1;
			unsigned _res1        : 1;
			unsigned _res2        : 1;
			unsigned _res3        : 1;
			unsigned _res4        : 1;
			unsigned _res5        : 1;
			unsigned _res6        : 1;
			unsigned _res7        : 1;
			unsigned _res8        : 1;
			unsigned _res9        : 1;
			unsigned _res10       : 1;
			unsigned _res11       : 1;
			unsigned _res12       : 1;
			unsigned _res13       : 1;
			unsigned _res14       : 1;
			unsigned _res15       : 1;
			unsigned _res16       : 1;
			unsigned _res17       : 1;
			unsigned rpid         : 1;
			unsigned _res18       : 1;
			unsigned _res19       : 1;
			unsigned _res20       : 1;
			unsigned _res21       : 1;
			unsigned _res22       : 1;
			unsigned _res23       : 1;
			unsigned _res24       : 1;
			unsigned sgx_lc       : 1;
			unsigned _res25       : 1;
		};

		uint32_t value;

	} ecx;

} cpu_features_ext_t;

typedef struct
{
	const char* name;
	uint32_t mask;

} feature_t;


const char* CacheTypeStrings[4] =
{
	NULL,
	"Data",
	"Instruction",
	"Unified",
};

const feature_t EdxFeatures[EDX_FEATURES_SIZE] =
{
	{ "FPU",           (1 <<  0) },
	{ "VME",           (1 <<  1) },
	{ "DE",            (1 <<  2) },
	{ "PSE",           (1 <<  3) },
	{ "TSC",           (1 <<  4) },
	{ "MSR",           (1 <<  5) },
	{ "PAE",           (1 <<  6) },
	{ "MCE",           (1 <<  7) },
	{ "CX8",           (1 <<  8) },
	{ "APIC",          (1 <<  9) },
	{ "SEP",           (1 << 11) },
	{ "MTRR",          (1 << 12) },
	{ "PGE",           (1 << 13) },
	{ "MCA",           (1 << 14) },
	{ "CMOV",          (1 << 15) },
	{ "PAT",           (1 << 16) },
	{ "PSE-36",        (1 << 17) },
	{ "PSN",           (1 << 18) },
	{ "CLFSH",         (1 << 19) },
	{ "DS",            (1 << 21) },
	{ "ACPI",          (1 << 22) },
	{ "MMX",           (1 << 23) },
	{ "FXSR",          (1 << 24) },
	{ "SSE",           (1 << 25) },
	{ "SSE2",          (1 << 26) },
	{ "SS",            (1 << 27) },
	{ "HTT",           (1 << 28) },
	{ "TM",            (1 << 29) },
	{ "PBE",           (1 << 31) }
};

const feature_t EcxFeatures[ECX_FEATURES_SIZE] =
{
	{ "SSE3",          (1 <<  0) },
	{ "PCLMULQDQ",     (1 <<  1) },
	{ "DTES64",        (1 <<  2) },
	{ "MONITOR",       (1 <<  3) },
	{ "DS-CPL",        (1 <<  4) },
	{ "VMX",           (1 <<  5) },
	{ "SMX",           (1 <<  6) },
	{ "EIST",          (1 <<  7) },
	{ "TM2",           (1 <<  8) },
	{ "SSSE3",         (1 <<  9) },
	{ "CNXT-ID",       (1 << 10) },
	{ "SDBG",          (1 << 11) },
	{ "FMA",           (1 << 12) },
	{ "CMPXCHG16B",    (1 << 13) },
	{ "XTPR",          (1 << 14) },
	{ "PDCM",          (1 << 15) },
	{ "PCID",          (1 << 17) },
	{ "DCA",           (1 << 18) },
	{ "SSE4.1",        (1 << 19) },
	{ "SSE4.2",        (1 << 20) },
	{ "X2APIC",        (1 << 21) },
	{ "MOVBE",         (1 << 22) },
	{ "POPCNT",        (1 << 23) },
	{ "TSC-DEADLINE",  (1 << 24) },
	{ "AESNI",         (1 << 25) },
	{ "XSAVE",         (1 << 26) },
	{ "OSXSAVE",       (1 << 27) },
	{ "AVX",           (1 << 28) },
	{ "F16C",          (1 << 29) },
	{ "RDRND",         (1 << 30) },
};

const feature_t EbxExtFeatures[EBX_EXT_FEATURES_SIZE] =
{
	{ "FSGSBASE",      (1 <<  0) },
	{ "IA32-TSC-ADJ",  (1 <<  1) },
	{ "SGX",           (1 <<  2) },
	{ "BMI1",          (1 <<  3) },
	{ "HLE",           (1 <<  4) },
	{ "AVX2",          (1 <<  5) },
	{ "FDP-EXCP",      (1 <<  6) },
	{ "SMEP",          (1 <<  7) },
	{ "BMI2",          (1 <<  8) },
	{ "ENHANCED-RMS",  (1 <<  9) },
	{ "INVPCID",       (1 << 10) },
	{ "RTM",           (1 << 11) },
	{ "RDT-M",         (1 << 12) },
	{ "DEPR-FCSDS",    (1 << 13) },
	{ "MPX",           (1 << 14) },
	{ "RDT-A",         (1 << 15) },
	{ "AVX512F",       (1 << 16) },
	{ "AVX512DQ",      (1 << 17) },
	{ "RDSEED",        (1 << 18) },
	{ "ADX",           (1 << 19) },
	{ "SMAP",          (1 << 20) },
	{ "CLFLUSHOPT",    (1 << 23) },
	{ "CLWB",          (1 << 24) },
	{ "INTEL-PTRACE",  (1 << 25) },
	{ "AVX512PF",      (1 << 26) },
	{ "AVX512ER",      (1 << 27) },
	{ "AVX512CD",      (1 << 28) },
	{ "SHA",           (1 << 29) },
	{ "AVX512BW",      (1 << 30) },
	{ "AVX512VL",      (1 << 31) }
};

const feature_t EcxExtFeatures[ECX_EXT_FEATURES_SIZE] =
{
	{ "PREFETCHWT1",   (1 <<  0) },
	{ "UMIP",          (1 <<  2) },
	{ "PKU",           (1 <<  3) },
	{ "OSPKE",         (1 <<  4) },
	{ "RPID",          (1 << 22) },
	{ "SGX-LC",        (1 << 30) }
};

const char* BrandStrings[256] =
{
	"<Unknow>",
	"Intel(R) Celeron(R) processor",
	"Intel(R) Pentium(R) III processor",
	"Intel(R) Pentium(R) III Xeon(R) processor",
	"Intel(R) Pentium(R) III processor",
	"<Unknow>",
	"Mobile Intel(R) Pentium(R) III processor-M",
	"Mobile Intel(R) Celeron(R) processor",
	"Intel(R) Pentium(R) 4 processor",
	"Intel(R) Pentium(R) 4 processor",
	"Intel(R) Celeron(R) processor",
	"Intel(R) Xeon(R) processor",
	"Intel(R) Xeon(R) processor MP",
	"<Unknow>",
	"Mobile Intel(R) Pentium(R) 4 processor-M",
	"Intel(R) Celeron(R) processor",
	"<Unknow>",
	"Mobile Genuine Intel(R) processor",
	"Intel(R) Celeron(R) M processor",
	"Mobile Intel(R) Celeron(R) processor",
	"Intel(R) Celeron(R) processor",
	"Mobile Genuine Intel(R) processor",
	"Intel(R) Pentium(R) M processor",
	"Mobile Intel(R) Celeron(R) processor",
	"<Unknow>"
};

const char* CacheTlbDescriptors[256] =
{
// 0x00
	NULL,
	"Instruction TLB: 4 KByte pages, 4-way set associative, 32 entries",
	"Instruction TLB: 4 MByte pages, fully associative, 2 entries",
	"Data TLB: 4 KByte pages, 4-way set associative, 64 entries",
	"Data TLB: 4 MByte pages, 4-way set associative, 8 entries",
	"Data TLB1: 4 MByte pages, 4-way set associative, 32 entries",
	"1st-level instruction cache: 8 KBytes, 4-way set associative, 32 byte line size",
	"1st-level instruction cache: 16 KBytes, 4-way set associative, 32 byte line size",
	"1st-level instruction cache: 32KBytes, 4-way set associative, 64 byte line size",
	"1st-level data cache: 8 KBytes, 2-way set associative, 32 byte line size",
	"Instruction TLB: 4 MByte pages, 4-way set associative, 4 entries",
	"1st-level data cache: 16 KBytes, 4-way set associative, 32 byte line size",
	"1st-level data cache: 16 KBytes, 4-way set associative, 64 byte line size",
	"1st-level data cache: 24 KBytes, 6-way set associative, 64 byte line size",
	NULL, NULL,
// 0x10
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	"2nd-level cache: 128 KBytes, 2-way set associative, 64 byte line size",
	NULL, NULL,
// 0x20
	NULL,
	"2nd-level cache: 256 KBytes, 8-way set associative, 64 byte line size",
	"3rd-level cache: 512 KBytes, 4-way set associative, 64 byte line size, 2 lines per sector",
	"3rd-level cache: 1 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector",
	"2nd-level cache: 1 MBytes, 16-way set associative, 64 byte line size",
	"3rd-level cache: 2 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector",
	NULL, NULL, NULL,
	"3rd-level cache: 4 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector",
	NULL, NULL,
	"1st-level data cache: 32 KBytes, 8-way set associative, 64 byte line size",
	NULL, NULL, NULL,
// 0x30
	"1st-level instruction cache: 32 KBytes, 8-way set associative, 64 byte line size",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
// 0x40
	"2nd-level cache or, if processor contains a valid 2nd-level cache, no 3rd-level cache",
	"2nd-level cache: 128 KBytes, 4-way set associative, 32 byte line size",
	"2nd-level cache: 256 KBytes, 4-way set associative, 32 byte line size",
	"2nd-level cache: 512 KBytes, 4-way set associative, 32 byte line size",
	"2nd-level cache: 1 MByte, 4-way set associative, 32 byte line size",
	"2nd-level cache: 2 MByte, 4-way set associative, 32 byte line size",
	"3rd-level cache: 4 MByte, 4-way set associative, 64 byte line size",
	"3rd-level cache: 8 MByte, 8-way set associative, 64 byte line size",
	"2nd-level cache: 3MByte, 12-way set associative, 64 byte line size",
	"3rd-level cache: 4MB, 16-way set associative, 64-byte line size (Intel Xeon processor MP, Family 0FH, Model 06H); 2nd-level cache: 4 MByte, 16-way set associative, 64 byte line size",
	"3rd-level cache: 6MByte, 12-way set associative, 64 byte line size",
	"3rd-level cache: 8MByte, 16-way set associative, 64 byte line size",
	"3rd-level cache: 12MByte, 12-way set associative, 64 byte line size",
	"3rd-level cache: 16MByte, 16-way set associative, 64 byte line size",
	"2nd-level cache: 6MByte, 24-way set associative, 64 byte line size",
	"Instruction TLB: 4 KByte pages, 32 entries",
// 0x50
	"Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 64 entries",
	"Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 128 entries",
	"Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 256 entries",
	NULL, NULL,
	"Instruction TLB: 2-MByte or 4-MByte pages, fully associative, 7 entries",
	"Data TLB0: 4 MByte pages, 4-way set associative, 16 entries",
	"Data TLB0: 4 KByte pages, 4-way associative, 16 entries",
	NULL,
	"Data TLB0: 4 KByte pages, fully associative, 16 entries",
	"Data TLB0: 2 MByte or 4 MByte pages, 4-way set associative, 32 entries",
	"Data TLB: 4 KByte and 4 MByte pages, 64 entries",
	"Data TLB: 4 KByte and 4 MByte pages,128 entries",
	"Data TLB: 4 KByte and 4 MByte pages,256 entries",
	NULL, NULL,
// 0x60
	"1st-level data cache: 16 KByte, 8-way set associative, 64 byte line size",
	"Instruction TLB: 4 KByte pages, fully associative, 48 entries",
	NULL,
	"Data TLB: 2 MByte or 4 MByte pages, 4-way set associative, 32 entries and a separate array with 1 GByte pages, 4-way set associative, 4 entries",
	"Data TLB: 4 KByte pages, 4-way set associative, 512 entries",
	NULL,
	"1st-level data cache: 8 KByte, 4-way set associative, 64 byte line size",
	"1st-level data cache: 16 KByte, 4-way set associative, 64 byte line size",
	"1st-level data cache: 32 KByte, 4-way set associative, 64 byte line size",
	NULL,
	"uTLB: 4 KByte pages, 8-way set associative, 64 entries",
	"DTLB: 4 KByte pages, 8-way set associative, 256 entries",
	"DTLB: 2M/4M pages, 8-way set associative, 128 entries",
	"DTLB: 1 GByte pages, fully associative, 16 entries",
	NULL, NULL,
// 0x70
	"Trace cache: 12 K-μop, 8-way set associative",
	"Trace cache: 16 K-μop, 8-way set associative",
	"Trace cache: 32 K-μop, 8-way set associative",
	NULL, NULL, NULL,
	"Instruction TLB: 2M/4M pages, fully associative, 8 entries",
	NULL,
	"2nd-level cache: 1 MByte, 4-way set associative, 64byte line size",
	"2nd-level cache: 128 KByte, 8-way set associative, 64 byte line size, 2 lines per sector",
	"2nd-level cache: 256 KByte, 8-way set associative, 64 byte line size, 2 lines per sector",
	"2nd-level cache: 512 KByte, 8-way set associative, 64 byte line size, 2 lines per sector",
	"2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size, 2 lines per sector",
	"2nd-level cache: 2 MByte, 8-way set associative, 64byte line size",
	NULL,
	"2nd-level cache: 512 KByte, 2-way set associative, 64-byte line size",
// 0x80
	"2nd-level cache: 512 KByte, 8-way set associative, 64-byte line size",
	NULL,
	"2nd-level cache: 256 KByte, 8-way set associative, 32 byte line size",
	"2nd-level cache: 512 KByte, 8-way set associative, 32 byte line size",
	"2nd-level cache: 1 MByte, 8-way set associative, 32 byte line size",
	"2nd-level cache: 2 MByte, 8-way set associative, 32 byte line size",
	"2nd-level cache: 512 KByte, 4-way set associative, 64 byte line size",
	"2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
// 0x90
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
// 0xA0
	"DTLB: 4k pages, fully associative, 32 entries",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
// 0xB0
	"Instruction TLB: 4 KByte pages, 4-way set associative, 128 entries",
	"Instruction TLB: 2M pages, 4-way, 8 entries or 4M pages, 4-way, 4 entries",
	"Instruction TLB: 4KByte pages, 4-way set associative, 64 entries",
	"Data TLB: 4 KByte pages, 4-way set associative, 128 entries",
	"Data TLB1: 4 KByte pages, 4-way associative, 256 entries",
	"Instruction TLB: 4KByte pages, 8-way set associative, 64 entries",
	"Instruction TLB: 4KByte pages, 8-way set associative, 128 entries",
	NULL, NULL, NULL,
	"Data TLB1: 4 KByte pages, 4-way associative, 64 entries",
	NULL, NULL, NULL, NULL, NULL,
// 0xC0
	"Data TLB: 4 KByte and 4 MByte pages, 4-way associative, 8 entries",
	"Shared 2nd-Level TLB: 4 KByte/2MByte pages, 8-way associative, 1024 entries",
	"DTLB: 4 KByte/2 MByte pages, 4-way associative, 16 entries",
	"Shared 2nd-Level TLB: 4 KByte /2 MByte pages, 6-way associative, 1536 entries. Also 1GBbyte pages, 4-way, 16 entries.",
	"DTLB: 2M/4M Byte pages, 4-way associative, 32 entries",
	NULL, NULL, NULL, NULL, NULL,
	"Shared 2nd-Level TLB: 4 KByte pages, 4-way associative, 512 entries",
	NULL, NULL, NULL, NULL, NULL,
// 0xD0
	"3rd-level cache: 512 KByte, 4-way set associative, 64 byte line size",
	"3rd-level cache: 1 MByte, 4-way set associative, 64 byte line size",
	"3rd-level cache: 2 MByte, 4-way set associative, 64 byte line size",
	NULL, NULL, NULL,
	"3rd-level cache: 1 MByte, 8-way set associative, 64 byte line size",
	"3rd-level cache: 2 MByte, 8-way set associative, 64 byte line size",
	"3rd-level cache: 4 MByte, 8-way set associative, 64 byte line size",
	NULL, NULL, NULL,
	"3rd-level cache: 1.5 MByte, 12-way set associative, 64 byte line size",
	"3rd-level cache: 3 MByte, 12-way set associative, 64 byte line size",
	"3rd-level cache: 6 MByte, 12-way set associative, 64 byte line size",
	NULL,
// 0xE0
	NULL, NULL,
	"3rd-level cache: 2 MByte, 16-way set associative, 64 byte line size",
	"3rd-level cache: 4 MByte, 16-way set associative, 64 byte line size",
	"3rd-level cache: 8 MByte, 16-way set associative, 64 byte line size",
	NULL, NULL, NULL, NULL, NULL,
	"3rd-level cache: 12MByte, 24-way set associative, 64 byte line size",
	"3rd-level cache: 18MByte, 24-way set associative, 64 byte line size",
	"3rd-level cache: 24MByte, 24-way set associative, 64 byte line size",
	NULL, NULL, NULL,
// 0xF0
	"64-Byte prefetching",
	"128-Byte prefetching",
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

