#pragma once

#include <map>

#include <common/EntityComponentSystem.h>

class ConnectionToClient;

class Connection2Entity {
public:

	entity_t getEntity(ConnectionToClient &connection) {
		entity_t &entity = connection2entity.at(&connection);
	    entity.sync();
	    return entity;
	}

	template < typename F >
	bool do_for(ConnectionToClient const &connection, F && f) {
		auto it = connection2entity.find(&connection);

		if (it == connection2entity.end())
			return false;

		it->second.sync();
		f(it->second);
		return true;
	}

	void erase(ConnectionToClient const &connection) {
		connection2entity.erase(&connection);
	}

	void insert(ConnectionToClient const &connection, entity_t entity) {
		connection2entity[&connection] = entity;
	}

private:
	std::map<ConnectionToClient const *, entity_t> connection2entity;
};
