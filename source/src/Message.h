#pragma once

#include <stdexcept>
#include <stdint.h>
#include <type_traits>
#include <vector>
#include <cstring> // std::memcpy

enum class Tag : uint32_t;

struct Message {
	using Data = std::vector<uint8_t>;

	Message() : tag(Tag(0)) {}
	Message(Tag tag, Data data = {}) :
		tag(tag),
		data(std::move(data))
	{
		;
	}

	Tag tag;
	Data data;
};

template < typename T >
void append(Message & m, T const & object) {
	static_assert(std::is_standard_layout<T>::value, "Can append only standard layout types");
	size_t sz = m.data.size();
	m.data.resize(sz + sizeof(object));
	std::memcpy(m.data.data() + sz, &object, sizeof(object));
}

class Deserializer {
	public:
		explicit Deserializer(Message const &m) : m(m) {}

		template < typename T >
		T take() {
			static_assert(std::is_standard_layout<T>::value, "Can take only standard layout types");
			if (sizeof(T) > m.data.size() + offset)
				throw std::length_error("Message is too short to contain desired type");

			T object;
			std::memcpy(&object, m.data.data() + offset, sizeof(object));
			offset += sizeof(object);
			return object;
		}

		bool empty() const {
			return offset == m.data.size();
		}

	private:
		Message const &m;
		size_t offset = 0;
};
