#pragma once

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

	bool operator==(EntityID const & other) const {return id == other.id;}

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(id);
	}
	// TODO separate. Client does not need to create new entities.
	static EntityID newID() {
		// TODO protect it with mutex
		last_id++; if(last_id == 0) last_id++;
		auto id = last_id;
		return EntityID(id);
	}

private:
	using Id = int; //std::uintmax_t;

	explicit EntityID(Id id) : id{id} {}

	Id id = 0;

	static Id last_id;
};
