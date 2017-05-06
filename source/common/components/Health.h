#pragma once

// Keep this separate from player.
// In principle, even obstacles can have health.
struct Health {
	int hp;

	template < typename Archive >
	void serialize(Archive & archive) {
		archive(hp);
	}

	bool alive() const {
		return hp > 0;
	}
};
