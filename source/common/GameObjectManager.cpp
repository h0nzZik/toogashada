#include "GameObjectManager.h"

#include <stdexcept>
#include <utility> // std::move

using namespace std;

GameObject & GameObjectManager::getObjectById(uint32_t id) {
	return *_objects_map.at(id);
}

uint32_t GameObjectManager::get_fresh_id() {
	return _last_id++;
}

void GameObjectManager::insert(std::unique_ptr<GameObject> object) {
	GameObject * o = object.get();
	if (_objects_map[o->object_id()] != nullptr)
		throw logic_error(string("2D Object with id ") + to_string(o->object_id()) + "is already registered");

	_objects_map[o->object_id()] = o;
	_objects.push_back(move(object));
}

GameObjectManager::iterator GameObjectManager::begin(){
	return _objects.begin();
}

GameObjectManager::iterator GameObjectManager::end(){
	return _objects.end();
}

GameObjectManager::const_iterator GameObjectManager::begin() const {
	return cbegin();
}

GameObjectManager::const_iterator GameObjectManager::end() const {
	return cend();
}

GameObjectManager::const_iterator GameObjectManager::cbegin() const {
	return _objects.cbegin();
}

GameObjectManager::const_iterator GameObjectManager::cend() const {
	return _objects.cend();
}
