// SPDX-License-Identifier: MIT
// Copyright (c) 2026 RealTimeChris
#pragma once

namespace lt {

	struct LT_ALIGN(64) uint64_aligner {
		LT_ALIGN(64) uint64 value{};

		LT_HOST consteval operator const uint64&() const {
			return value;
		}
	};

	enum class cpu_property_types : uint64 {
		l1_cache_size,
		l2_cache_size,
		l3_cache_size,
		cpu_arch_index,
		thread_count,
		alignment,
		arg_alignment,
	};

	struct cpu_properties {
	public:	
		static constexpr uint64_aligner values[]{ { 49152ULL },{ 2097152ULL },{ 37748736ULL },{ 1ULL },{ 32ULL },{ 32ULL },{ 64ULL } };

		LT_HOST_DEVICE static consteval const uint64& get_value(cpu_property_types index) {

			return values[static_cast<uint64>(index)].value;
		}
	};

}
