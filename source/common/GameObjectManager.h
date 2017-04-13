#pragma once

// Standard
#include <map>
#include <vector>
#include <memory>

// Boost
#include <boost/iterator/indirect_iterator.hpp>

// Project
#include "GameObject.h"

class GameObjectManager {
private:
	using Objects = std::vector<std::unique_ptr<GameObject>>;
	using ObjectMap = std::map<uint32_t, GameObject *>;

	public:
		GameObject & getObjectById(uint32_t id);

		uint32_t get_fresh_id();
		void insert(std::unique_ptr<GameObject> object);

		// Only constant iterator is needed

		using iterator = boost::indirect_iterator<typename Objects::iterator, GameObject>;
		using const_iterator = boost::indirect_iterator<typename Objects::const_iterator, GameObject>;

		iterator begin();
		iterator end();

		const_iterator begin() const;
		const_iterator end() const;

		const_iterator cbegin() const;
		const_iterator cend() const;


	private:
		Objects _objects;
		ObjectMap _objects_map;
		uint32_t _last_id = 0;
};


