#pragma once

struct EntityMotion {
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(left, right, up, down);
	}
};
