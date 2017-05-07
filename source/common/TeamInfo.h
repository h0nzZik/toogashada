//
// Created by lukas on 05/05/17.
//

#ifndef TOOGASHADA_TEAMINFO_H
#define TOOGASHADA_TEAMINFO_H

struct TeamInfo {

  std::string mName;
  int mId;
  int mPlayerCount;

  template <typename Archive> void serialize(Archive &archive) {
    archive(mName, mId, mPlayerCount);
  }
};

#endif // TOOGASHADA_TEAMINFO_H
