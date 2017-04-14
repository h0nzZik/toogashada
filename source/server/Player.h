#pragma once

#include <string>

class GameObject;

class Player {
public:
	using PlayerId = uint32_t;

	Player(PlayerId id, GameObject & object) :
		_id(id), _object(object){}

	std::string name;
	PlayerId id() const { return _id; }
	GameObject & gameObject() const { return _object; }

private:
	PlayerId _id;
	GameObject & _object;
};
