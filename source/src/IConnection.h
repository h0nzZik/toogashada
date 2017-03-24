#pragma once

#include <functional>
#include "Message.h"

class IConnection {
	public:
		virtual ~IConnection() = default;
		virtual void send(Message message) = 0;
		virtual void listen(std::function<void(Message)> handle) = 0;
};

