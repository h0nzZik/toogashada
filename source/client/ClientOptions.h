#pragma once

#include <string>

#include "ClientOptions_fwd.h"

struct ClientOptions {
  std::string ip;
  std::string port;
  std::string playerName;
  std::string playerTeam;
  int windowWidth;
  int windowHeight;
};
