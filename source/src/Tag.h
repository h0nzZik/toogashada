#pragma once

#include <stdint.h>

enum class Tag : uint32_t {
	Hello, // Both client and server can send this.
	Ping, // Can be used to measure RTT.
	Pong, // reply to ping, with the same data
	GoodBye, // Sent by both client and server before shutting the connection down.
	NewPolygonalObject, // Sent by server. Data: 4B ID of object
	ObjectPosition, // 4B object ID, position, speed
};


