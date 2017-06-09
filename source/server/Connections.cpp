// standard
#include <memory>  // std::unique_ptr
#include <utility> // std::swap
#include <vector>

// server
#include <server/ConnectionToClient.hpp>

// self
#include <server/Connections.h>

using namespace std;

class Connections::Impl {
public:
  Impl() = default;
  ~Impl() = default;

  ConnectionToClient &insert(unique_ptr<ConnectionToClient> connection);
  void erase(ConnectionToClient &connection);
  void for_each(function<void(ConnectionToClient &)> f);
  void handle_erase(function<void(ConnectionToClient &)> f);

private:
  bool isToBeCleared(ConnectionToClient const &connection) const;
  vector<unique_ptr<ConnectionToClient>> connections;
  vector<ConnectionToClient *> connectionsToClear;
};

ConnectionToClient &
Connections::Impl::insert(unique_ptr<ConnectionToClient> connection) {
  ConnectionToClient *con = connection.get();
  connections.push_back(move(connection));
  return *con;
}

bool Connections::Impl::isToBeCleared(
    ConnectionToClient const &connection) const {
  return find(connectionsToClear.begin(), connectionsToClear.end(),
              &connection) != connectionsToClear.end();
}

void Connections::Impl::erase(ConnectionToClient &connection) {
  if (!isToBeCleared(connection))
    connectionsToClear.push_back(&connection);
}

void Connections::Impl::for_each(function<void(ConnectionToClient &)> f) {
  for (unique_ptr<ConnectionToClient> const &conn : connections) {
    // This is not needed in the time of writing,
    // but it might be useful later
    if (isToBeCleared(*conn))
      continue;

    f(*conn);
  }
}

/*
 * If the callback does not throw, then this does not throw, either.
 * Otherwise it provides normal exception guarantee.
 */
void Connections::Impl::handle_erase(function<void(ConnectionToClient &)> f) {

  auto end = connectionsToClear.end();
  for (auto it = connectionsToClear.begin(); it != end; ++it) {
    ConnectionToClient *toClear = *it;
    connectionsToClear.erase(it);

    // Find the unique pointer
    auto found = find_if(connections.begin(), connections.end(),
                         [&](auto const &u) { return u.get() == toClear; });
    assert(found != connections.end());

    unique_ptr<ConnectionToClient> connection;
    connection.swap(*found);
    connections.erase(found);

    // Until now, no exception could occur.
    // Now it can, but we do not mind it.
    f(*toClear);
  }
}

/*************************************
 * pImpl outer
 */

Connections::Connections() { pImpl = make_unique<Impl>(); }

Connections::~Connections() = default;

ConnectionToClient &
Connections::insert(unique_ptr<ConnectionToClient> connection) {
  return pImpl->insert(move(connection));
}

void Connections::erase(ConnectionToClient &connection) {
  return pImpl->erase(connection);
}

void Connections::for_each(function<void(ConnectionToClient &)> f) {
  return pImpl->for_each(move(f));
}

void Connections::handle_erase(function<void(ConnectionToClient &)> f) {
  return pImpl->handle_erase(move(f));
}
