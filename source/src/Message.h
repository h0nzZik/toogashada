#pragma once

#include <ostream>
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

std::ostream & operator<<(std::ostream & o, Message const &m);

template < typename T >
void append(Message & m, T const & object) {
	static_assert(std::is_pod<T>::value, "Can append only standard layout types");
	size_t sz = m.data.size();
	m.data.resize(sz + sizeof(object));
	std::memcpy(m.data.data() + sz, &object, sizeof(object));
}

inline void append(Message &m, std::string const & str) {
	// TODO assert that the string is short
	append(m, uint32_t(str.length()));
	for(char c : str)
		append(m, c);
}

template < typename T >
void append(Message &m, std::vector<T> const & vec) {
	// TODO assert that the vector is short
	append(m, uint32_t(vec.size()));
	for(T const & val : vec)
		append(m, val);
}

class Deserializer {
	public:
		explicit Deserializer(Message const &m) : m(m) {}

		template < typename T >
		T take() {
			static_assert(std::is_pod<T>::value, "Can take only standard layout types");
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

template < typename T > struct Take {
	static T from(Deserializer &d) {
		return d.take<T>();
	}
};

template<>
struct Take<std::string> {
	static std::string  from(Deserializer &d) {
		uint32_t const len = Take<uint32_t>::from(d);
		std::string s;
		s.reserve(len);
		for (size_t i = 0; i < len; i++)
			s.push_back(Take<char>::from(d));
		return s;
	}
};

template < typename T >
struct Take<std::vector<T>> {
	static std::vector<T> from(Deserializer &d) {
		std::vector<T> v;
		uint32_t const len = Take<uint32_t>::from(d);
		v.reserve(len);
		for (size_t i = 0; i < len; i++)
			v.push_back(Take<T>::from(d));
		return v;
	}
};

