// standard
#include <set>
#include <utility> // std::swap

// server
#include <server/ConnectionToClient.hpp>

// self
#include <server/Connections.h>

using namespace std;

class Connections::Impl {
public:
  Impl() = default;
  ~Impl();
  ConnectionToClient &insert(unique_ptr<ConnectionToClient> connection);
  void erase(ConnectionToClient &connection);
  void for_each(function<void(ConnectionToClient &)> f);
  void handle_erase(function<void(ConnectionToClient &)> f);

private:
  set<ConnectionToClient *> connections;
  set<ConnectionToClient *> connectionsToClear;
};

Connections::Impl::~Impl() {
  for (ConnectionToClient *conn : connections)
    delete conn;
}

ConnectionToClient &
Connections::Impl::insert(unique_ptr<ConnectionToClient> connection) {
  ConnectionToClient *con = connection.release();
  connections.insert(con);
  return *con;
}

void Connections::Impl::erase(ConnectionToClient &connection) {
  connectionsToClear.insert(&connection);
}

void Connections::Impl::for_each(function<void(ConnectionToClient &)> f) {
  for (ConnectionToClient *conn : connections) {
    // This is not needed in the time of writing,
    // but it might be useful later
    if (connectionsToClear.find(conn) != connectionsToClear.end())
      continue;

    f(*conn);
  }
}

// It is not really exception safe
void Connections::Impl::handle_erase(function<void(ConnectionToClient &)> f) {
  set<ConnectionToClient *> localConnectionsToClear;
  using std::swap;
  swap(localConnectionsToClear, connectionsToClear);

  // Clear them
  for (ConnectionToClient *toClear : localConnectionsToClear) {
    connections.erase(toClear);
  }

  // Callback
  for (ConnectionToClient *toClear : localConnectionsToClear) {
    f(*toClear);
  }

  // Real desotroy
  for (ConnectionToClient *toClear : localConnectionsToClear) {
    delete toClear;
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
