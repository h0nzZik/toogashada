#pragma once

#include <boost/variant/variant.hpp>

#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>

struct ServerMessage {
  using Data = boost::variant<MsgGameInfo, MsgNewPlayer, MsgNewEntity,
                              MsgUpdateEntity, MsgDeleteEntity,
                              MsgPlayerAssignedEntityId>;

  Data data;

  static constexpr Tag tag = Tag::UniversalServerMessage;

  template <typename Archive> void serialize(Archive &archive) {
    archive(data);
  }

  Message to_message() const;

  static ServerMessage from(Message const &msg);
};
