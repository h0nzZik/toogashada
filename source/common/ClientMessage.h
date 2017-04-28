#pragma once

#include <boost/variant/variant.hpp>

#include <common/Tag.h>
#include <common/Message.h>
#include <common/Messages.h>

struct ClientMessage {
	using Data = boost::variant<
			MsgIntroduceMyPlayer,
			MsgPlayerMovesLeft,
			MsgPlayerMovesRight,
			MsgPlayerMovesForward,
			MsgPlayerMovesBackward
			>;

	Data data;

	static constexpr Tag tag = Tag::UniversalClientMessage;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(data);
	}

	Message to_message() const;

	static ClientMessage from(Message const &msg);
};
