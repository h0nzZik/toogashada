//
// Created by lukas on 05/05/17.
//

#pragma once

#include "iostream"
#include <map>
#include <tuple>
#include <string>
#include "cereal/types/tuple.hpp"
#include "TeamInfo.h"

struct GameInfo {

    Scalar mapW;
    Scalar mapH;
    // name, <id, playerCount>
    std::vector<TeamInfo> teams;

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(mapW, mapH, teams);
    }


    auto findTeam(const std::string& teamName) {

        return std::find_if(teams.begin(), teams.end(), [&](TeamInfo teamInfo){ return teamInfo.mName == teamName; });
    }

    void rebuildIds() {

        int i = 0;
        for (auto &team : teams) {

            team.mId = i;
            ++i;
        }
    }

    void addPlayer(const PlayerInfo& info) {

        if (findTeam(info.mTeam) == teams.end()) {

            teams.push_back({info.mTeam, 0, 0});
            rebuildIds();
        }

        findTeam(info.mTeam)->mPlayerCount++;
    }

    void removePlayer(const PlayerInfo& info) {

        if (findTeam(info.mTeam) == teams.end()) {

            std::cerr << "Team " << info.mTeam << " not found when removing player." << std::endl;
        }

        auto team = findTeam(info.mTeam);
        if ((team->mPlayerCount--) == 0) {

            teams.erase(team);
            rebuildIds();
        }
    }

    geometry::RectangularArea getArea() const {
        return {{0,0,},{mapW, mapH}};
    };

    Scalar width() const {
        return mapW;
    }

    Scalar height() const {
        return mapH;
    }
};
