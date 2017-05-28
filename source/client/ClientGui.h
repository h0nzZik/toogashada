#pragma once

#include <string>
#include <memory>
#include <common/TeamInfo.h>

struct EntityComponentSystem;
struct EntityDrawer;
class ClientController;

class ClientGui {
	public:
		ClientGui(ClientController &controller, const std::string &playerName,
            const std::string &playerTeam, int windowWidth = -1,
            int windowHeight = -1);
		~ClientGui();

		void setMapSize(int w, int h);
		void setTeamInfo(const std::vector<TeamInfo> &teamInfo);
		void renderGui(EntityComponentSystem &entities);
		geometry::Point getScreenCoords(const geometry::Point &point) const;

	private:
		class Impl;
		std::unique_ptr<Impl> impl;
};

