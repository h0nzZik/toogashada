#pragma once

#include <string>

struct PlayerInfo {
    PlayerInfo() = default;

    PlayerInfo(std::string name, std::string team) : mName(name), mTeam(team) {
        std::cout << "Player " << mName << " of team " << mTeam << std::endl;
    }

    std::string mName;
    std::string mTeam;

    template < typename Archive >
    void serialize(Archive & archive) {
        archive(mName, mTeam);
    }
};
