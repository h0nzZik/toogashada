#include <string>
#include <sstream>

#include <cereal/archives/binary.hpp>
#include <cereal/types/boost_variant.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <common/ServerMessage.h>

#include "Tag.h"
#include "AnyComponent.h"

// self

using namespace std;

Message ServerMessage::to_message() const {
	std::stringstream ss; // any stream can be used
	cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

	oarchive(*this);
	string s = ss.str();
	return Message{Tag::UniversalServerMessage, {s.begin(), s.end()}};
}

ServerMessage ServerMessage::from(Message const &msg) {
	if (msg.tag != tag)
		throw std::domain_error("Message tag does not fit MsgNewPolygonalObject");

	std::string s;
	for (char c : msg.data)
		s.push_back(c);

	std::stringstream ss{s};

	cereal::BinaryInputArchive iarchive(ss);
	ServerMessage um;
	iarchive(um);
	return um;
}



