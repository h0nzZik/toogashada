#pragma once

#include <stdint.h>
#include <vector>

enum class Tag : uint32_t;

struct Message {
	using Data = std::vector<uint8_t>;

	Tag tag;
	Data data;
};
