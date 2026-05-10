// SPDX-License-Identifier: MIT
// Copyright (c) 2026 RealTimeChris
// cmake/detection/main.cpp

#if defined(LT_DETECT_CPU_PROPERTIES)
	#include <cstring>
	#include <cstdint>
	#include <cstdlib>
	#include <iostream>
	#include <thread>
	#include <vector>

	#if defined(__aarch64__) || defined(_M_ARM64)
		#if defined(__linux__)
			#include <sys/auxv.h>
			#include <asm/hwcap.h>
		#elif defined(__APPLE__)
			#include <sys/sysctl.h>
		#endif
	#else

		#if defined(_MSC_VER)
			#include <intrin.h>
		#elif defined(__GNUC__) || defined(__clang__)
			#include <cpuid.h>
		#endif
	#endif

	#if defined(_WIN32) || defined(_WIN64)
		#include <Windows.h>
	#endif

	#if defined(__linux__) || defined(__ANDROID__)
		#include <fstream>
		#include <string>
	#endif

	#if defined(__APPLE__) && defined(__MACH__)
		#include <sys/sysctl.h>
		#include <sys/types.h>
		#include <string>
	#endif

enum class instruction_sets {
	fallback = 0x0,
	avx_2	 = 0x1,
	avx_512	 = 0x2,
	neon	 = 0x4,
	sve_2	 = 0x8,
};

enum class cache_level {
	one	  = 1,
	two	  = 2,
	three = 3,
};

	#if defined(__aarch64__) || defined(_M_ARM64)
inline static uint32_t detect_supported_architectures() {
	uint32_t host_isa = static_cast<uint32_t>(instruction_sets::neon);

		#if defined(__linux__) && defined(HWCAP_SVE)
	unsigned long hwcap = getauxval(AT_HWCAP);
	if (hwcap & HWCAP_SVE) {
		host_isa |= static_cast<uint32_t>(instruction_sets::sve_2);
	}
		#endif

	return host_isa;
}

	#elif defined(__x86_64__) || defined(_M_X64)
static constexpr uint32_t cpuid_avx2_bit	 = 1ul << 5;
static constexpr uint32_t cpuid_avx512_bit	 = 1ul << 16;
static constexpr uint64_t cpuid_avx256_saved = 1ULL << 2;
static constexpr uint64_t cpuid_avx512_saved = 7ULL << 5;
static constexpr uint32_t cpuid_osx_save	 = (1ul << 26) | (1ul << 27);

inline static void cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
		#if defined(_MSC_VER)
	int32_t cpu_info[4];
	__cpuidex(cpu_info, *eax, *ecx);
	*eax = cpu_info[0];
	*ebx = cpu_info[1];
	*ecx = cpu_info[2];
	*edx = cpu_info[3];
		#else
	uint32_t a = *eax, b, c = *ecx, d;
	asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(a), "c"(c));
	*eax = a;
	*ebx = b;
	*ecx = c;
	*edx = d;
		#endif
}

inline static uint64_t xgetbv() {
		#if defined(_MSC_VER)
	return _xgetbv(0);
		#else
	uint32_t eax, edx;
	asm volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
	return (( uint64_t )edx << 32) | eax;
		#endif
}

inline static uint32_t detect_supported_architectures() {
	std::uint32_t eax	   = 0;
	std::uint32_t ebx	   = 0;
	std::uint32_t ecx	   = 0;
	std::uint32_t edx	   = 0;
	std::uint32_t host_isa = static_cast<uint32_t>(instruction_sets::fallback);

	eax = 0x1;
	ecx = 0x0;
	cpuid(&eax, &ebx, &ecx, &edx);

	if ((ecx & cpuid_osx_save) != cpuid_osx_save) {
		return host_isa;
	}

	uint64_t xcr0 = xgetbv();
	if ((xcr0 & cpuid_avx256_saved) == 0) {
		return host_isa;
	}

	eax = 0x7;
	ecx = 0x0;
	cpuid(&eax, &ebx, &ecx, &edx);

	if (ebx & cpuid_avx2_bit) {
		host_isa |= static_cast<uint32_t>(instruction_sets::avx_2);
	}

	if (!((xcr0 & cpuid_avx512_saved) == cpuid_avx512_saved)) {
		return host_isa;
	}

	if (ebx & cpuid_avx512_bit) {
		host_isa |= static_cast<uint32_t>(instruction_sets::avx_512);
	}

	return host_isa;
}

	#else
inline static uint32_t detect_supported_architectures() {
	return static_cast<uint32_t>(instruction_sets::fallback);
}
	#endif

inline uint64_t get_cache_size(cache_level level) {
	#if defined(_WIN32) || defined(_WIN64)
	DWORD bufferSize = 0;
	std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer{};
	GetLogicalProcessorInformation(nullptr, &bufferSize);
	if (bufferSize == 0)
		return 0;
	buffer.resize(bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

	if (!GetLogicalProcessorInformation(buffer.data(), &bufferSize)) {
		return 0;
	}

	for (const auto& i: buffer) {
		if (i.Relationship == RelationCache && i.Cache.Level == static_cast<int32_t>(level)) {
			if (level == cache_level::one && i.Cache.Type == CacheData) {
				return i.Cache.Size;
			} else if (level != cache_level::one && i.Cache.Type == CacheUnified) {
				return i.Cache.Size;
			}
		}
	}
	return 0;

	#elif defined(__linux__) || defined(__ANDROID__)
	auto get_cache_size_from_file = [](const std::string& index) -> uint64_t {
		const std::string cacheFilePath = "/sys/devices/system/cpu/cpu0/cache/index" + index + "/size";
		std::ifstream file(cacheFilePath);
		if (!file.is_open())
			return 0ULL;

		std::string sizeStr;
		file >> sizeStr;
		uint64_t size = std::stoul(sizeStr);
		if (sizeStr.find('K') != std::string::npos)
			size *= 1024;
		else if (sizeStr.find('M') != std::string::npos)
			size *= 1024 * 1024;
		return static_cast<uint64_t>(size);
	};

	if (level == cache_level::one)
		return get_cache_size_from_file("0");
	std::string idx = (level == cache_level::two) ? "2" : "3";
	return get_cache_size_from_file(idx);

	#elif defined(__APPLE__)
	auto get_cache_size_for_mac = [](const char* cacheType) {
		size_t cacheSize  = 0;
		size_t size		  = sizeof(cacheSize);
		std::string query = std::string("hw.") + cacheType + "cachesize";
		if (sysctlbyname(query.c_str(), &cacheSize, &size, nullptr, 0) != 0)
			return size_t{};
		return cacheSize;
	};

	if (level == cache_level::one)
		return get_cache_size_for_mac("l1d");
	if (level == cache_level::two)
		return get_cache_size_for_mac("l2");
	return get_cache_size_for_mac("l3");
	#endif

	return 0;
}

enum class host_cxx_compilers {
	clang,
	gnu,
	msvc,
};

int main() {
	const uint32_t thread_count	 = std::thread::hardware_concurrency();
	const uint32_t supported_isa = detect_supported_architectures();
	const uint64_t l1_cache_size = get_cache_size(cache_level::one);
	const uint64_t l2_cache_size = get_cache_size(cache_level::two);
	const uint64_t l3_cache_size = get_cache_size(cache_level::three);

	uint32_t cpu_arch_index = 0;
	if (supported_isa == static_cast<uint32_t>(instruction_sets::avx_512)) {
		cpu_arch_index = 2;
	} else if (supported_isa == static_cast<uint32_t>(instruction_sets::avx_2)) {
		cpu_arch_index = 1;
	} else if (supported_isa == static_cast<uint32_t>(instruction_sets::sve_2)) {
		cpu_arch_index = 2;
	} else if (supported_isa == static_cast<uint32_t>(instruction_sets::neon)) {
		cpu_arch_index = 1;
	} else {
		cpu_arch_index = 0;
	}

	std::cout << "L1_CACHE_SIZE=" << l1_cache_size << std::endl;
	std::cout << "L2_CACHE_SIZE=" << l2_cache_size << std::endl;
	std::cout << "L3_CACHE_SIZE=" << l3_cache_size << std::endl;
	std::cout << "CPU_ARCH_INDEX=" << cpu_arch_index << std::endl;
	std::cout << "THREAD_COUNT=" << thread_count << std::endl;
	std::cout << "HAS_AVX2=" << ((supported_isa & static_cast<uint32_t>(instruction_sets::avx_2)) ? 1ULL : 0) << std::endl;
	std::cout << "HAS_AVX512=" << ((supported_isa & static_cast<uint32_t>(instruction_sets::avx_512)) ? 1ULL : 0) << std::endl;
	std::cout << "HAS_NEON=" << ((supported_isa & static_cast<uint32_t>(instruction_sets::neon)) ? 1ULL : 0) << std::endl;
	std::cout << "HAS_SVE2=" << ((supported_isa & static_cast<uint32_t>(instruction_sets::sve_2)) ? 1ULL : 0) << std::endl;
	std::cout << "CPU_SUCCESS=1" << std::endl;
	return 0;
}
#else
int main() {
	return -1;
}
#endif
