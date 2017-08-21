
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

#include <stdio.h>

#include "archinfo.h"

// max cpuid leaves
uint32_t MaxLeaf, MaxExtLeaf;


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

void maximum_leaves();
void vendor();
void brand();
void features();
void cache_tlb();
void cache();

int main(int argc, char* argv[])
{
	// check if the cpuid instruction is available
	if(!cpuid_available())
	{
		printf("CPUID is not supported\n");
	
		return 1;
	}

	// get maximum cpuid leaves
	maximum_leaves();

	// print the vendor id
	vendor();

	// print brand informations
	brand();

	// print the features
	features();

	// print cache and tlb informations
	cache_tlb();

	// print the cache informations
	cache();

	return 0;
}

void maximum_leaves()
{
	uint32_t ebx, ecx, edx;

	CPUID(0x0, MaxLeaf, ebx, ecx, edx);
	CPUID(0x80000000, MaxExtLeaf, ebx, ecx, edx);
}

void vendor()
{
	uint32_t eax;
	cpu_vendor_t vendor = { 0 };

	// get the vendor id string
	CPUID(0x0, eax, vendor.dword0, vendor.dword2, vendor.dword1);

	// print the vendor id
	printf("Vendor ID: %s\n\n", vendor.id);
}

void brand()
{
	uint32_t ebx, ecx, edx;
	cpu_signature_t signature;

	// get cpuid leaf 0x1 informations
	CPUID(0x1, signature.value, ebx, ecx, edx);

	// check the maximum cpuid extension leaf
	if(MaxExtLeaf < 0x80000004)
	{
		// use the brand string table
		const char* brand = BrandStrings[ebx & 0xFF];

		if(brand != NULL)
			printf("Brand: %s\n", brand);
		else
			printf("Brand: <Unknow>\n");
	}
	else
	{
		uint32_t brand[12];

		// get the brand string
		CPUID(0x80000002, brand[ 0], brand[ 1], brand[ 2], brand[ 3]);
		CPUID(0x80000003, brand[ 4], brand[ 5], brand[ 6], brand[ 7]);
		CPUID(0x80000004, brand[ 8], brand[ 9], brand[10], brand[11]);

		printf("Brand: %s\n", (char*)brand);
	}

	printf("\nStepping ID: %X\n", signature.stepping);
	printf("Model: %X\n", signature.model);
	printf("Family: %X\n", signature.family);

	if(signature.family == 0x6 || signature.family == 0xF)
		printf("Extended Model: %X\n", (signature.model_ext << 4) | signature.model);

	if(signature.family != 0xF)
		printf("Extended Family: %X\n", signature.family_ext + signature.family);

	printf("\n");
}

void features()
{
	uint64_t xcr0;
	uint32_t eax, ebx, edx;

	cpu_features_t features;
	cpu_features_ext_t features_ext;

	// get the cpu features
	CPUID(0x1, eax, ebx, features.ecx.value, features.edx.value);

	// check the osxsave feature and set the xcr0 register state
	xcr0 = (features.ecx.osxsave) ? xcr0_state() : 0;

	// check the avx feature bit validity
	features.ecx.avx &= (xcr0 & 0x6) == 0x6;

	// check the f16c and fma feature bits validity
	features.ecx.f16c &= features.ecx.avx;
	features.ecx.fma  &= features.ecx.avx;

	printf("Features:\n");

	// print the features encoded in edx
	for(uint32_t i = 0; i < EDX_FEATURES_SIZE; ++i)
		if(features.edx.value & EdxFeatures[i].mask)
			printf("%s ", EdxFeatures[i].name);

	// print the features encoded in ecx
	for(uint32_t i = 0; i < ECX_FEATURES_SIZE; ++i)
		if(features.ecx.value & EcxFeatures[i].mask)
			printf("%s ", EcxFeatures[i].name);

	printf("\n\n");

	// check the maximum cpuid leaf
	if(MaxLeaf < 0x7)
		return;

	// get the cpu extended features
	CPUID_EXT(0x7, 0x0, eax, features_ext.ebx.value, features_ext.ecx.value, edx);

	// check the avx2 feature bit validity
	features_ext.ebx.avx2 &= features.ecx.avx;

	// check the avx512 feature bits validity
	features_ext.ebx.avx512f  &= (xcr0 & 0xE6) == 0xE6;
	features_ext.ebx.avx512dq &= features_ext.ebx.avx512f;
	features_ext.ebx.avx512pf &= features_ext.ebx.avx512f;
	features_ext.ebx.avx512er &= features_ext.ebx.avx512f;
	features_ext.ebx.avx512cd &= features_ext.ebx.avx512f;
	features_ext.ebx.avx512bw &= features_ext.ebx.avx512f;
	features_ext.ebx.avx512vl &= features_ext.ebx.avx512f;

	printf("Extended Features:\n");

	// print the extended features encoded in ebx
	for(uint32_t i = 0; i < EBX_EXT_FEATURES_SIZE; ++i)
		if(features_ext.ebx.value & EbxExtFeatures[i].mask)
			printf("%s ", EbxExtFeatures[i].name);

	// print the extended features encoded in ecx
	for(uint32_t i = 0; i < ECX_EXT_FEATURES_SIZE; ++i)
		if(features.ecx.value & EcxExtFeatures[i].mask)
			printf("%s ", EcxExtFeatures[i].name);

	printf("\n\n");
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

int cache_info(cpu_cache_t* cache, uint32_t subleaf)
{
	uint32_t eax, ebx, ecx, edx;

	// get cache informations
	CPUID_EXT(0x4, subleaf, eax, ebx, ecx, edx);

	cache->level = (eax >> 5) & 0x7;
	cache->type = eax & 0x3;
	cache->params_lo = ebx + 0x401001;
	cache->params_hi = ecx + 1;
	cache->size = cache->sets * cache->line_size * cache->partitions * cache->ways;

	return eax;
}

void cache()
{
	// check the maximum cpuid leaf
	if(MaxLeaf < 0x4)
		return;

	cpu_cache_t cache;
	uint32_t subleaf = 0;

	// check every cache
	while(cache_info(&cache, subleaf))
	{
		printf("Cache Level %d %s:\n", cache.level, CacheTypeStrings[cache.type]);
		printf("Size: %d KB, ", cache.size >> 10);
		printf("%d partitions\n", cache.partitions);
		printf("%d sets, ", cache.sets);
		printf("%d-way set associative, ", cache.ways);
		printf("%d byte line size\n\n", cache.line_size);

		subleaf++;
	}
}

