#include <stdexcept>
#include <utility> // std::move
#include "TwoDimensionalObjectManager.h"

using namespace std;

TwoDimensionalObject & TwoDimensionalObjectManager::getObjectById(uint32_t id) {
	return *_objects_map.at(id);
}

uint32_t TwoDimensionalObjectManager::get_fresh_id() {
	return _last_id++;
}

void TwoDimensionalObjectManager::insert(std::unique_ptr<TwoDimensionalObject> object) {
	TwoDimensionalObject * o = object.get();
	if (_objects_map[o->object_id()] != nullptr)
		throw logic_error(string("2D Object with id ") + to_string(o->object_id()) + "is already registered");

	_objects_map[o->object_id()] = o;
	_objects.push_back(move(object));
}


