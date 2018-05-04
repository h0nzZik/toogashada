#include <iostream>

#include <boost/program_options.hpp>

// Project
#include "Client.h"
#include "ClientOptions.h"

using namespace boost::program_options;

boost::program_options::options_description client_options() {
  boost::program_options::options_description desc("Options");

  desc.add_options()("ip,i", value<std::string>()->default_value("localhost"),
                     "Server address");
  desc.add_options()("port,p", value<std::string>()->default_value("2061"),
                     "Server port");
  desc.add_options()("player-name,n", value<std::string>()->required());
  desc.add_options()("player-team,t", value<std::string>());
  desc.add_options()("window-width,w", value<int>());
  desc.add_options()("window-height,h", value<int>());

  return desc;
}

ClientOptions parseClientOptions(int argc, char const *argv[]) noexcept {
  options_description desc = client_options();

  try {
    variables_map vm;

    store(command_line_parser(argc, argv).options(desc).run(), vm);
    notify(vm);

    // Looking forward to C++20 designated initializers
    ClientOptions opts{
        vm["ip"].as<std::string>(),
        vm["port"].as<std::string>(),
        vm["player-name"].as<std::string>(),
        vm.count("player-team") ? vm["player-team"].as<std::string>() : "",
        vm.count("window-width") ? vm["window-width"].as<int>() : -1,
        vm.count("window-height") ? vm["window-height"].as<int>() : -1};

    return opts;
  } catch (std::exception const &e) {
    std::cerr << "Cannot parse arguments:\n"
              << e.what() << std::endl
              << std::endl;
    std::cerr << desc << std::endl;
    std::exit(1);
  }
}

void runClientWithOptions(ClientOptions const &options) {
  try {
    Client client{options};
    client.run();
  } catch (std::exception &e) {
    std::cerr << "Error: \n" << e.what() << std::endl << std::endl;
    std::exit(1);
  }
}

int main(int argc, const char **argv) {
  std::cout << "Toogashada Client" << std::endl;
  ClientOptions options = parseClientOptions(argc, argv);
  runClientWithOptions(options);
}
