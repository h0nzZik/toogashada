#pragma once

#include <string>

struct ClientPlayer {
	ClientPlayer(std::string name, std::string team) :
		mName(name),
		mTeam(team)
	{
		std::cout << "Player " << mName << " of team " << mTeam <<std::endl;
	}

	std::string mName;
	std::string mTeam;
};

