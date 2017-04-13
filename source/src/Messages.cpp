#include <stdexcept>
#include "Messages.h"

using namespace std;

Message MsgNewPolygonalObject::to_message() const {
	if (shape.size() > 255)
		throw std::length_error("Polygonal objects can consist of 255 points at most.");

	Message msg;
	msg.tag = tag;

	append(msg, object_id);
	append(msg, center);
	msg.data.push_back(uint8_t(shape.size()));
	for (IntPoint const & p : shape) {
		append(msg, p);
	}

	return msg;
}

MsgNewPolygonalObject MsgNewPolygonalObject::from(Message msg) {
	if (msg.tag != tag)
		throw std::domain_error("Message tag does not fit MsgNewPolygonalObject");

	Deserializer d{msg};

	MsgNewPolygonalObject npo;
	npo.object_id = d.take<uint32_t>();
	npo.center = d.take<IntPoint>();
	npo.shape.resize(d.take<uint8_t>());
	for (IntPoint & p : npo.shape)
		p = d.take<IntPoint>();

	if (!d.empty())
		throw std::range_error("Message should be now empty, but it is not!");

	return npo;
}
