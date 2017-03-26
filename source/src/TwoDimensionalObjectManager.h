#pragma once

#include <map>
#include <vector>
#include <memory>
#include "TwoDimensionalObject.h"

class TwoDimensionalObjectManager {
	public:
		TwoDimensionalObject & getObjectById(uint32_t id);

		uint32_t get_fresh_id();
		void insert(std::unique_ptr<TwoDimensionalObject> object);

	private:
		std::vector<std::unique_ptr<TwoDimensionalObject>> _objects;
		std::map<uint32_t, TwoDimensionalObject *> _objects_map;
		uint32_t _last_id = 0;
};


