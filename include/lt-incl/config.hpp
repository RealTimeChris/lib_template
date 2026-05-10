// SPDX-License-Identifier: MIT
// Copyright (c) 2026 RealTimeChris
// lib_template-incl/config.hpp

#pragma once

#ifndef LT_DEBUG
	#if defined(_DEBUG) || (defined(NDEBUG) == 0) || defined(DEBUG)
		#define LT_DEBUG 1
	#else
		#define LT_DEBUG 0
	#endif
#endif

#if !defined(LT_LIKELY)
	#define LT_LIKELY(...) (__VA_ARGS__) [[likely]]
#endif

#if !defined(LT_UNLIKELY)
	#define LT_UNLIKELY(...) (__VA_ARGS__) [[unlikely]]
#endif

#if !defined(LT_ELSE_UNLIKELY)
	#define LT_ELSE_UNLIKELY(...) __VA_ARGS__ [[unlikely]]
#endif

#if !defined(LT_ALIGN)
	#define LT_ALIGN(b) alignas(b)
#endif

#include <functional>
#include <algorithm>
#include <concepts>
#include <charconv>
#include <cstring>
#include <cstdint>
#include <vector>
#include <array>
#include <bit>

namespace lib_template {}