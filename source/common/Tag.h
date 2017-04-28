#pragma once

#include <stdint.h>

enum class Tag : uint32_t {
	_Universal = 0x00,
	Hello, // Both client and server can send this.
	Ping, // Can be used to measure RTT.
	Pong, // reply to ping, with the same data
	GoodBye, // Sent by both client and server before shutting the connection down.

	_ServerToClient = 0x100,
	UniversalServerMessage, // will eventually replace all others
	/* Messages from server to client */
	NewPlayer,

	NewEntity,
	UpdateEntity,
	DeleteEntity,


	_ClientToServer = 0x200,
	UniversalClientMessage, // will eventually replace all others
	/* Messages from client to server */
    IntroduceMyPlayer, // sent after connection is made
	PlayerMovesLeft,
	PlayerMovesRight,
	PlayerMovesBackward,
	PlayerMovesForward,
	PlayerStops
};


