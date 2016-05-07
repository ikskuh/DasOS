#pragma once

#define ENUM_CLASS_OPERATORS(type) static inline type operator | (type lhs, type rhs) { \
	return static_cast<type>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)); \
} \
static inline type operator & (type lhs, type rhs) { \
	return static_cast<type>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)); \
} \
static inline bool operator * (type lhs, type rhs) { \
	return (static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)) != 0; \
} \
static inline type & operator |=(type & lhs, type rhs) { \
	reinterpret_cast<uint32_t&>(lhs) |= static_cast<uint32_t>(rhs); \
	return lhs; \
} 



