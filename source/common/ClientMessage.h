#pragma once

#include <boost/variant/variant.hpp>

#include <common/Message.h>
#include <common/Messages.h>
#include <common/Tag.h>

struct ClientMessage {
  using Data = boost::variant<MsgIntroduceMyPlayer, MsgPlayerActionChange,
                              MsgPlayerRotation>;

  Data data;

  static constexpr Tag tag = Tag::UniversalClientMessage;

  template <typename Archive> void serialize(Archive &archive) {
    archive(data);
  }

  Message to_message() const;

  static ClientMessage from(Message const &msg);
};
