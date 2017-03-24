#pragma once

#include <stdint.h>
#include <vector>

enum class Tag : uint32_t;

struct Message {
	Tag tag;
	std::vector<uint8_t> data;
};
