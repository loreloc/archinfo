
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
#include <stdlib.h>
#include <string.h>

#include "archinfo.h"

// maximum cpuid leaf
uint32_t MaxLeaf = 0;

// maximum cpuid extended leaf
uint32_t MaxExtLeaf = 0;

// xcr0 register state
uint32_t XCR0 = 0;

// cpu features
cpu_features_t features = { 0 };

// cpu extended features
cpu_features_ext_t features_ext = { 0 };


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
void cache_tlb();
void caches();
void ext_features();
void frequencies();

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

	// print informations about the cache and the tlb
	cache_tlb();

	// print informations about the caches
	caches();

	return 0;
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

const char* microarch_info(uint32_t model_num)
{
	// https://software.intel.com/en-us/articles/intel-architecture-and-processor-identification-with-cpuid-model-and-family-numbers
	// https://en.wikipedia.org/wiki/List_of_Intel_CPU_microarchitectures
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
		return "Haswell - 22 nm";
	case 0x3D:
		return "Broadwell - 14 nm";
	case 0x5E:
		return "Skylake - 14 nm";
	case 0x9E:
		return "KabyLake - 14 nm";

	default:
		return "<Unknow>";
	}
}

void sign_brand_features()
{
	cpu_signature_t signature;
	uint32_t brand_index;

	// get the cpu signature, brand index and basic features
	CPUID(0x1, signature.value, brand_index, features.ecx.value, features.edx.value);

	uint32_t brand[12] = { 0 };

	// check the maximum cpuid extension leaf
	if(MaxExtLeaf < 0x80000004)
	{
		// copy the brand string
		strcpy((char*)brand, BrandStrings[brand_index & 0xFF]);
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
	XCR0 = (features.ecx.osxsave) ? xcr0_state() : 0;

	// check the avx feature bit validity
	features.ecx.avx &= (XCR0 & 0x6) == 0x6;
		
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

void caches()
{
	// check the maximum cpuid leaf
	if(MaxLeaf < 0x4)
		return;

	cpu_cache_t cache;
	uint32_t subleaf = 0;

	// retrieve informations about every cache
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

void ext_features()
{
	// check the maximum cpuid leaf
	if(MaxLeaf < 0x7)
		return;

	uint32_t eax, edx;

	// get the cpu extended features
	CPUID_EXT(0x7, 0x0, eax, features_ext.ebx.value, features_ext.ecx.value, edx);

	// check the avx2 feature bit validity
	features_ext.ebx.avx2 &= features.ecx.avx;

	// check the avx512 feature bits validity
	features_ext.ebx.avx512f  &= (XCR0 & 0xE6) == 0xE6;
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
		if(features_ext.ecx.value & EcxExtFeatures[i].mask)
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

