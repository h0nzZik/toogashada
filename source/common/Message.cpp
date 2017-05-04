#include "Message.h"

std::ostream &operator<<(std::ostream &o, Message const &m) {
  o << "{tag: " << uint32_t(m.tag) << ", data: [";

  bool first = true;
  for (uint8_t b : m.data) {
    if (!first)
      o << " ";
    first = false;

    o << std::hex << static_cast<int>(b);
  }

  o << "]}";
  return o;
}
