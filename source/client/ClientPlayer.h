#ifndef SRC_CLIENTPLAYER_H_
#define SRC_CLIENTPLAYER_H_

#include <common/Angle.h>
#include <common/GPoint.h>

struct ClientPlayer {
	ClientPlayer() :
		position{0,0},
		speed{0,0},
		angularPosition{0},
		angularSpeed{0}
	{
		;
	}

	Point position;
	Point speed;
	Angle angularPosition;
	AngularSpeed angularSpeed;
};

#endif /* SRC_CLIENTPLAYER_H_ */
