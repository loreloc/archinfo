
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

#if   defined(_WIN32)
	#include <windows.h>
#elif defined(__linux__)
	#define _GNU_SOURCE
	#include <unistd.h>
	#include <sched.h>
	#include <pthread.h>
#else
	#error "Platform not supported!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archinfo.h"

// maximum cpuid leaf
uint32_t MaxLeaf = 0;

// maximum cpuid extended leaf
uint32_t MaxExtLeaf = 0;

// xcr0 register state
uint64_t XCR0 = 0;

// cpu features
cpu_features_t Features = { 0 };

// cpu extended features
cpu_features_ext_t FeaturesExt = { 0 };


uint64_t xcr0_state()
{
	uint32_t eax, edx;

	__asm__
	(
		"xorl %%ecx, %%ecx\n\t"
		"xgetbv\n\t"
		: "=a"(eax), "=d"(edx)
		:
		: "ecx"
	);

	return ((uint64_t)edx << 32) | eax;
}

int cpuid_available()
{
#ifdef __x86_64__

	return 1;

#else

	int val;

	// http://wiki.osdev.org/CPUID
	__asm__
	(
		"pushfl\n\t"
		"pushfl\n\t"
		"xorl $0x200000, (%%esp)\n\t"
		"popfl\n\t"
		"pushfl\n\t"
		"popl %%eax\n\t"
		"xorl (%%esp), %%eax\n\t"
		"popfl\n\t"
		"andl $0x200000, %%eax\n\t"
		: "=a"(val)
	);

	return val;

#endif
}

void max_leaf_vendor();
void max_ext_leaf();
void sign_brand_features();
void ext_features();
void frequencies();
void topology();
void single_core_topology();
void multi_core_topology();
void cache_tlb();


int main(int argc, char* argv[])
{
	// check if the cpuid instruction is available
	if(!cpuid_available())
	{
		printf("CPUID is not supported\n");
	
		return 1;
	}

	// get the maximum cpuid leaf and print the cpu vendor id
	max_leaf_vendor();

	// get the maximum cpuid extended leaf
	max_ext_leaf();

	// print the signature, the brand and the features of the cpu
	sign_brand_features();

	// print the extended features of the cpu
	ext_features();

	// print the cpu base and maximum frequencies and the bus frequency
	frequencies();

	// print informations about the cpu topology
	if(!Features.edx.htt)
		single_core_topology();
	else
		multi_core_topology();

	// print informations about the cache and the tlb
	cache_tlb();

	return 0;
}

const char* microarch_info(uint32_t model_num)
{
	// https://software.intel.com/en-us/articles/intel-architecture-and-processor-identification-with-cpuid-model-and-family-numbers
	// https://en.wikipedia.org/wiki/List_of_Intel_CPU_microarchitectures
	// http://instlatx64.atw.hu/
	//
	// TODO: Older microarchitectures identification

	switch(model_num)
	{
	// atom microarchitectures
	case 0x1C:
	case 0x26:
		return "Atom - 45 nm";
	case 0x36:
		return "Atom - 32 nm";

	// mainline microarchitectures
	case 0x03:
	case 0x04:
		return "Prescott - 90 nm";
	case 0x06:
		return "Presler - 65 nm";
	case 0x0D:
		return "Dothan - 90 nm";
	case 0x0F:
	case 0x16:
		return "Merom - 65 nm";
	case 0x17:
	case 0x1D:
		return "Penryn - 45 nm";
	case 0x1A:
	case 0x1E:
	case 0x2E:
		return "Nehalem - 45 nm";
	case 0x25:
	case 0x2C:
	case 0x2F:
		return "Westmere - 32 nm";
	case 0x2A:
	case 0x2D:
		return "SandyBridge - 32 nm";
	case 0x3A:
		return "IvyBridge - 22 nm";
	case 0x3C:
	case 0x3F:
		return "Haswell - 22 nm";
	case 0x3D:
	case 0x4F:
		return "Broadwell - 14 nm";
	case 0x55:
	case 0x5E:
		return "Skylake - 14 nm";
	case 0x8E:
	case 0x9E:
		return "KabyLake - 14 nm";

	default:
		return "<Unknow>";
	}
}

uint32_t fast_log2(uint32_t x)
{
	uint32_t y;

	__asm__
	(
		"bsr %1, %0\n\t"
		: "=r"(y)
		: "r"(x)
	);

	return y;
}

uint32_t round_next_pow2(uint32_t x)
{
	x--;
	x |= x >>  1;
	x |= x >>  2;
	x |= x >>  4;
	x |= x >>  8;
	x |= x >> 16;
	x++;

	return x;
}

uint32_t find(uint32_t* v, uint32_t n, uint32_t val)
{
	for(uint32_t i = 0; i < n; ++i)
		if(v[i] == val)
			return i;

	return n;
}

uint32_t apic_id()
{
	uint32_t eax, ebx, ecx, edx;

	// get the apic id of the current logical processor
	CPUID(0x1, eax, ebx, ecx, edx);

	return ebx >> 24;
}

int cache_info(cpu_cache_t* cache, uint32_t subleaf)
{
	uint32_t eax, ebx, ecx, edx;

	// get cache informations
	CPUID_EXT(0x4, subleaf, eax, ebx, ecx, edx);

	uint32_t type = eax & 0x3;

	if(!type)
		return 0;

	cache->level = (eax >> 5) & 0x7;
	cache->type = CacheTypeStrings[type];

	cache->line_size = (ebx & 0xFFF) + 1;
	cache->partitions = ((ebx >> 12) & 0x3FF) + 1;
	cache->ways = (ebx >> 22) + 1;
	cache->sets = ecx + 1;

	cache->size = cache->sets * cache->line_size * cache->partitions * cache->ways;

	uint32_t max_share = round_next_pow2(((eax >> 14) & 0xFFF) + 1);
	uint32_t cache_mask_width = fast_log2(max_share);

	cache->mask = ~((-1) << cache_mask_width);

	return 1;
}

void print_cache_info(cpu_cache_t cache)
{
	printf("Cache Level %u %s:\n", cache.level, cache.type);
	printf("Size: %u KB, %u partitions\n", cache.size >> 10, cache.partitions);
	printf("%u sets, %u-way set associative, %u byte line size\n\n", cache.sets, cache.ways, cache.line_size);
}

void print_cache_tlb_info(uint32_t reg)
{
	const char* info0 = CacheTlbDescriptors[(reg      ) & 0xFF];
	const char* info1 = CacheTlbDescriptors[(reg >>  8) & 0xFF];
	const char* info2 = CacheTlbDescriptors[(reg >> 16) & 0xFF];
	const char* info3 = CacheTlbDescriptors[(reg >> 24) & 0xFF];

	if(info0 != NULL) printf("%s\n", info0);
	if(info1 != NULL) printf("%s\n", info1);
	if(info2 != NULL) printf("%s\n", info2);
	if(info3 != NULL) printf("%s\n", info3);
}

void max_leaf_vendor()
{
	cpu_vendor_t vendor = { 0 };

	// get the maximum cpuid leaf and cpu vendor id
	CPUID(0x0, MaxLeaf, vendor.dword0, vendor.dword2, vendor.dword1);

	// print the vendor id
	printf("Vendor ID: %s\n\n", vendor.id);
}

void max_ext_leaf()
{
	uint32_t ebx, ecx, edx;

	// get the maximum cpuid extended leaf
	CPUID(0x80000000, MaxExtLeaf, ebx, ecx, edx);
}

void sign_brand_features()
{
	cpu_signature_t signature;
	uint32_t ebx;

	// get the cpu signature, brand index and basic features
	CPUID(0x1, signature.value, ebx, Features.ecx.value, Features.edx.value);

	uint32_t brand[12] = { 0 };

	// check the maximum cpuid extension leaf
	if(MaxExtLeaf < 0x80000004)
	{
		uint32_t brand_index = ebx & 0xFF;

		// copy the brand string
		strcpy((char*)brand, BrandStrings[brand_index]);
	}
	else
	{
		// get the brand string
		CPUID(0x80000002, brand[ 0], brand[ 1], brand[ 2], brand[ 3]);
		CPUID(0x80000003, brand[ 4], brand[ 5], brand[ 6], brand[ 7]);
		CPUID(0x80000004, brand[ 8], brand[ 9], brand[10], brand[11]);
	}

	// print the brand string
	printf("Brand: %s\n\n", (char*)brand);

	// print the stepping id, model and family of the cpu
	printf("Stepping ID: %X\n", signature.stepping);
	printf("Model: %X\n", signature.model);
	printf("Family: %X\n", signature.family);

	uint32_t model_num = signature.model;

	// check the extended model number
	if(signature.family == 0x6 || signature.family == 0xF)
	{
		model_num |= (signature.model_ext << 4);

		printf("Extended Model: %X\n", model_num);
	}

	// check the extended family
	if(signature.family != 0xF)
		printf("Extended Family: %X\n", signature.family_ext + signature.family);

	printf("\n");

	// print informations about the microarchitecture
	printf("Microarchitecture: %s\n\n", microarch_info(model_num));

	// check the osxsave feature and set the xcr0 register state
	XCR0 = (Features.ecx.osxsave) ? xcr0_state() : 0;

	// check the avx feature bit validity
	Features.ecx.avx &= (XCR0 & 0x6) == 0x6;
		
	// check the f16c and fma feature bits validity
	Features.ecx.f16c &= Features.ecx.avx;
	Features.ecx.fma  &= Features.ecx.avx;

	printf("Features:\n");

	// print the features encoded in edx
	for(uint32_t i = 0; i < EDX_FEATURES_SIZE; ++i)
		if(Features.edx.value & EdxFeatures[i].mask)
			printf("%s ", EdxFeatures[i].name);

	// print the features encoded in ecx
	for(uint32_t i = 0; i < ECX_FEATURES_SIZE; ++i)
		if(Features.ecx.value & EcxFeatures[i].mask)
			printf("%s ", EcxFeatures[i].name);

	printf("\n\n");
}

void ext_features()
{
	// check the maximum cpuid leaf
	if(MaxLeaf < 0x7)
		return;

	uint32_t eax, edx;

	// get the cpu extended features
	CPUID_EXT(0x7, 0x0, eax, FeaturesExt.ebx.value, FeaturesExt.ecx.value, edx);

	// check the avx2 feature bit validity
	FeaturesExt.ebx.avx2 &= Features.ecx.avx;

	// check the avx512 feature bits validity
	FeaturesExt.ebx.avx512f  &= (XCR0 & 0xE6) == 0xE6;
	FeaturesExt.ebx.avx512dq &= FeaturesExt.ebx.avx512f;
	FeaturesExt.ebx.avx512pf &= FeaturesExt.ebx.avx512f;
	FeaturesExt.ebx.avx512er &= FeaturesExt.ebx.avx512f;
	FeaturesExt.ebx.avx512cd &= FeaturesExt.ebx.avx512f;
	FeaturesExt.ebx.avx512bw &= FeaturesExt.ebx.avx512f;
	FeaturesExt.ebx.avx512vl &= FeaturesExt.ebx.avx512f;

	printf("Extended Features:\n");

	// print the extended features encoded in ebx
	for(uint32_t i = 0; i < EBX_EXT_FEATURES_SIZE; ++i)
		if(FeaturesExt.ebx.value & EbxExtFeatures[i].mask)
			printf("%s ", EbxExtFeatures[i].name);

	// print the extended features encoded in ecx
	for(uint32_t i = 0; i < ECX_EXT_FEATURES_SIZE; ++i)
		if(FeaturesExt.ecx.value & EcxExtFeatures[i].mask)
			printf("%s ", EcxExtFeatures[i].name);

	printf("\n\n");
}

void frequencies()
{
	// check the maximum cpuid leaf
	if(MaxLeaf < 0x16)
		return;

	uint32_t eax, ebx, ecx, edx;

	// get frequencies informations
	CPUID(0x16, eax, ebx, ecx, edx);

	if(eax != 0)
		printf("Cpu base frequency: %f GHz\n", (float)eax / 1000.0f);

	if(ebx != 0)
		printf("Cpu maximum frequency: %f GHz\n", (float)ebx / 1000.0f);

	if(ecx != 0)
		printf("Bus (reference) frequency: %f MHz\n", (float)ecx);

	printf("\n");
}

void single_core_topology()
{
	// print the number of cores and the number of threads
	printf("Cores: %u\n", 1);
	printf("Threads: %u\n\n", 1);

	// check the maximum cpuid leaf
	if(MaxLeaf < 0x4)
		return;

	uint32_t subleaf = 0;
	cpu_cache_t cache;

	// retrieve informations about every cache
	while(cache_info(&cache, subleaf))
	{
		// print the cache informations
		print_cache_info(cache);

		subleaf++;
	}
}

void multi_core_topology()
{
	uint32_t eax, ebx, ecx, edx;

	// get the maximum number of logical processors
	CPUID(0x1, eax, ebx, ecx, edx);
	uint32_t max_logical_proc = (ebx >> 16) & 0xFF;

	// get the maximum number of physical processors
	CPUID_EXT(0x4, 0x0, eax, ebx, ecx, edx);
	uint32_t max_physical_proc = (eax >> 26) + 1;

	// calculate the mask of the smt sub id
	uint32_t smt_mask_width = fast_log2(round_next_pow2(max_logical_proc) / max_physical_proc);
	uint32_t smt_mask = ~((-1) << smt_mask_width);

	// calculate the mask of the core sub id
	uint32_t core_mask_width = fast_log2(max_physical_proc);
	uint32_t core_mask = (~((-1) << (core_mask_width + smt_mask_width))) ^ smt_mask;

	uint32_t threads_cnt;
	uint32_t apic_ids[256];

#if   defined(_WIN32)
	// get the number of logical processors
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	threads_cnt = sysinfo.dwNumberOfProcessors;

	// get the main thread handle
	HANDLE thread = GetCurrentThread();

	// save the previous affinity mask
	DWORD_PTR prev_affinity_mask = SetThreadAffinityMask(thread, 1);

	// get the first apic id
	apic_ids[0] = apic_id();

	// for each logical processor get the apic id
	for(uint32_t i = 1; i < threads_cnt; ++i)
	{
		// set the affinity to a logical processor
		SetThreadAffinityMask(thread, 1 << i);

		// get the apic id
		apic_ids[i] = apic_id();
	}

	// set the previous affinity mask
	SetThreadAffinityMask(thread, prev_affinity_mask);
#elif defined(__linux__)
	// get the number of logical processors
	threads_cnt = sysconf(_SC_NPROCESSORS_ONLN);
		
	// get the main thread handle
	pthread_t thread = pthread_self();

	// save the previous affinity
	cpu_set_t prev_cpu_set;
	pthread_getaffinity_np(thread, sizeof(cpu_set_t), &prev_cpu_set);

	// for each logical procesor get the apic id
	for(uint32_t i = 0; i < threads_cnt; ++i)
	{
		// set the affinity to a logical processor
		cpu_set_t cpu_set;
		CPU_ZERO(&cpu_set);
		CPU_SET(i, &cpu_set);
		pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpu_set);

		// get the apic id
		apic_ids[i] = apic_id();
	}

	// set the previous affinity
	pthread_setaffinity_np(thread, sizeof(cpu_set_t), &prev_cpu_set);
#endif

	uint32_t cores_ids[256];
	uint32_t cores_cnt = 0;

	// find the number of cores
	for(uint32_t i = 0; i < threads_cnt; ++i)
	{
		// calculate the core id
		uint32_t core_id = (apic_ids[i] & core_mask) >> smt_mask_width;

		// check for already found core ids
		if(find(cores_ids, cores_cnt, core_id) == cores_cnt)
		{
			cores_ids[cores_cnt] = core_id;

			cores_cnt++;
		}
	}

	// calculate the package mask
	uint32_t pkg_mask = core_mask | smt_mask;

	cpu_cache_t cache;
	uint32_t subleaf = 0;

	// unshared caches array
	cpu_cache_t unshared_caches[8];
	uint32_t unshared_caches_cnt = 0;

	// shared caches array
	cpu_cache_t shared_caches[8];
	uint32_t shared_caches_cnt = 0;

	// retrieve informations about every cache
	while(cache_info(&cache, subleaf))
	{
		// check if the cache is shared between all the cores
		if(cache.mask == pkg_mask)
		{
			shared_caches[shared_caches_cnt] = cache;

			shared_caches_cnt++;
		}
		else
		{
			unshared_caches[unshared_caches_cnt] = cache;

			unshared_caches_cnt++;
		}

		subleaf++;
	}

	// print the number of cores and the number of threads
	printf("Cores: %u\n", cores_cnt);
	printf("Threads: %u\n\n", threads_cnt);

	// print the unshared caches of each core
	for(uint32_t i = 0; i < cores_cnt; ++i)
	{
		printf("Core #%u\n", cores_ids[i]);

		for(uint32_t k = 0; k < unshared_caches_cnt; ++k)
			printf("   Cache Level %u %s\n", unshared_caches[k].level, unshared_caches[k].type);
	}

	if(shared_caches_cnt != 0)
	{
		printf("\nShared Caches:\n");

		// print the shared caches
		for(uint32_t k = 0; k < shared_caches_cnt; ++k)
			printf("   Cache Level %u %s\n", shared_caches[k].level, shared_caches[k].type);
	}

	printf("\n");
		
	// print the details of all the unshared caches
	for(uint32_t i = 0; i < unshared_caches_cnt; ++i)
		print_cache_info(unshared_caches[i]);

	// print the details of all the shared caches
	for(uint32_t i = 0; i < shared_caches_cnt; ++i)
		print_cache_info(shared_caches[i]);
}

void cache_tlb()
{
	// check the maximum cpuid leaf
	if(MaxLeaf < 0x2)
		return;

	uint32_t eax, ebx, ecx, edx;

	// get cache and tlb informations
	CPUID(0x2, eax, ebx, ecx, edx);

	printf("Cache and TLB informations:\n");

	if(~eax & 0x80000000)
		print_cache_tlb_info(eax >> 8);

	if(~ebx & 0x80000000)
		print_cache_tlb_info(ebx);
	
	if(~ecx & 0x80000000)
		print_cache_tlb_info(ecx);

	if(~edx & 0x80000000)
		print_cache_tlb_info(edx);

	printf("\n");
}

