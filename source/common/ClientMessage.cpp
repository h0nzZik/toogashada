#include <string>
#include <sstream>

#include <cereal/archives/binary.hpp>
#include <cereal/types/boost_variant.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <common/ClientMessage.h>

#include "Tag.h"
#include "AnyComponent.h"

// self

using namespace std;

Message ClientMessage::to_message() const {
	std::stringstream ss; // any stream can be used
	cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

	oarchive(*this);
	string s = ss.str();
	return Message{Tag::UniversalClientMessage, {s.begin(), s.end()}};
}

ClientMessage ClientMessage::from(Message const &msg) {
	if (msg.tag != tag)
		throw std::domain_error("Message tag does not fit.");

	std::string s;
	for (char c : msg.data)
		s.push_back(c);

	std::stringstream ss{s};

	cereal::BinaryInputArchive iarchive(ss);
	ClientMessage um;
	iarchive(um);
	return um;
}



