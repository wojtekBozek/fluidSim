#pragma once
#include "vertex.hpp"

namespace wojtek_math {
	constexpr size_t golden_ratio = 0x9e3779b9;
	template <typename T>
	inline void combined_hash(size_t& seed, const T& value)
	{
		seed ^= std::hash<T>()(value) + golden_ratio + (seed << 6) + (seed >> 2);
	}
	inline size_t vertexHash(const vertex3D& vertex)
	{
		size_t hash_value = 0;
		combined_hash(hash_value, static_cast<uint32_t>(vertex.xyz.x));
		combined_hash(hash_value, static_cast<uint32_t>(vertex.xyz.y));
		combined_hash(hash_value, static_cast<uint32_t>(vertex.xyz.z));
		combined_hash(hash_value, static_cast<uint32_t>(vertex.normal.x));
		combined_hash(hash_value, static_cast<uint32_t>(vertex.normal.y));
		combined_hash(hash_value, static_cast<uint32_t>(vertex.normal.z));
		combined_hash(hash_value, static_cast<uint32_t>(vertex.uv.x));
		combined_hash(hash_value, static_cast<uint32_t>(vertex.uv.y));
		std::cout << "hash value of Vertex: " << hash_value << "\n";
		return hash_value;
	}
}