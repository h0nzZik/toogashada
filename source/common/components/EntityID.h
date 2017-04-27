#pragma once

#include <cstdint>

/**
 * This component allows us to synchronize entity managers
 * on clients and server. It represents the unique identifier
 * of an entity.
 */
class EntityID {
public:
	EntityID() = default;
	EntityID(EntityID const &) = default;
	EntityID & operator=(EntityID const &) = default;

	bool operator==(EntityID const & other) const {return _id == other._id;}
	bool operator<(EntityID const & other) const {return _id < other._id;}

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(_id);
	}
	// TODO separate. Client does not need to create new entities.
	static EntityID newID() {
		// TODO protect it with mutex
		last_id++; if(last_id == 0) last_id++;
		auto id = last_id;
		return EntityID(id);
	}


	using Id = std::uintmax_t;
	Id id() const { return _id; }
private:

	explicit EntityID(Id id) : _id{id} {}

	Id _id = 0;

	static Id last_id;
};
