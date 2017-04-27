#include <stdexcept>
#include "Messages.h"

using namespace std;

// TODO reduce duplicated code

Message MsgNewPolygonalObject::to_message() const {
	if (shape.size() > 255)
		throw std::length_error("Polygonal objects can consist of 255 points at most.");

	Message msg;
	msg.tag = tag;

	append(msg, object_id);
	append(msg, center);
	append(msg, shape);

	return msg;
}

MsgNewPolygonalObject MsgNewPolygonalObject::from(Message const & msg) {
	if (msg.tag != tag)
		throw std::domain_error("Message tag does not fit MsgNewPolygonalObject");

	Deserializer d{msg};

	MsgNewPolygonalObject npo;
	npo.object_id = Take<uint32_t>::from(d);
	npo.center = Take<Point>::from(d);
	npo.shape = Take<vector<Point>>::from(d);

	if (!d.empty())
		throw std::range_error("Message should be now empty, but it is not!");

	return npo;
}

Message MsgObjectPosition::to_message() const {
	Message msg;
	msg.tag = tag;
	append(msg, *this);
	return msg;
}

MsgObjectPosition MsgObjectPosition::from(Message const &msg) {
	if (msg.tag != tag)
		throw std::domain_error("Message tag does not fit MsgObjectPosition");

	Deserializer d{msg};

	MsgObjectPosition mop = d.take<MsgObjectPosition>();

	if (!d.empty())
		throw std::range_error("Message should be now empty, but it is not!");

	return mop;
}

Message MsgNewPlayer::to_message() const {
	if (playerName.length() > 255)
		throw std::length_error("Player name can be at most 255 characters long");

	if (playerTeam.length() > 255)
		throw std::length_error("Player team can be at most 255 characters long");

	Message msg;
	msg.tag = tag;
	append(msg, player_id);
	append(msg, object_id);
	append(msg, playerName);
	append(msg, playerTeam);
	return msg;
}

MsgNewPlayer MsgNewPlayer::from(Message const & msg) {
	if (msg.tag != tag)
		throw std::domain_error("Message tag does not fit MsgNewPolygonalObject");

	Deserializer d{msg};
	MsgNewPlayer mnp;
	mnp.player_id = Take<uint32_t>::from(d);
	mnp.object_id = Take<uint32_t>::from(d);
	mnp.playerName = Take<string>::from(d);
	mnp.playerTeam = Take<string>::from(d);

	if (!d.empty())
		throw std::range_error("Message should be now empty, but it is not!");

	return mnp;
}

Message MsgSetPlayerMovement::to_message() const {
	Message msg;
	msg.tag = tag;
	append(msg, *this);
	return msg;
}

MsgSetPlayerMovement MsgSetPlayerMovement::from(Message const & msg)
{
	if (msg.tag != tag)
		throw std::domain_error("Message tag does not fit MsgNewPolygonalObject");

	Deserializer d{msg};
	MsgSetPlayerMovement movement = d.take<MsgSetPlayerMovement>();

	if (!d.empty())
		throw std::range_error("Message should be now empty, but it is not!");

	return movement;
}

Message MsgIntroduceMyPlayer::to_message() const {

    Message msg;
    msg.tag = tag;
    append(msg, playerName);
    append(msg, playerTeam);
    return msg;
}

MsgIntroduceMyPlayer MsgIntroduceMyPlayer::from(Message const &msg) {

    if (msg.tag != tag)
        throw std::domain_error("Message tag does not fit MsgNewPolygonalObject");

    Deserializer d{msg};
    MsgIntroduceMyPlayer introduction;

    introduction.playerName = Take<std::string>::from(d);
    introduction.playerTeam = Take<std::string>::from(d);

    if (!d.empty())
        throw std::range_error("Message should be now empty, but it is not!");

    return introduction;

}
