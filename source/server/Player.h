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

	enum class Movement {
		None, Forward, Backward, Left, Right
	};

private:
	PlayerId _id;
	GameObject & _object;
};
