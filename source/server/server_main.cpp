#include <csignal>
#include <iostream>

#include <boost/program_options.hpp>

#include "Server.h"

using namespace std;
using namespace boost::program_options;

static Server *p_server = nullptr;

static void on_signal(int signal) {
  if (!p_server)
    return;

  cout << "Signal " << signal << " caught." << endl;
  p_server->shutdown();
}

void server(int port) {
  cout << "Starting server on port " << port << endl;
  Server server(port);
  p_server = &server;
  std::signal(SIGINT, on_signal);

  server.run();
  cout << "Server exited normally" << endl;
}

int main(int argc, char const *argv[]) {
  try {
    options_description options{"Options"};
    options.add_options()("help,h", "Help screen")(
        "port", value<short>()->default_value(2061), "Port");

    variables_map vm;
    store(parse_command_line(argc, argv, options), vm);

    if (vm.count("help")) {
      std::cout << options << '\n';
      return 0;
    }

    server(vm["port"].as<short>());
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
}
