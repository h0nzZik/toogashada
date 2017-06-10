#include <iostream>

#include <boost/program_options.hpp>

// Project
#include "Client.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, const char **argv) {

  options_description desc("Options");
  variables_map vm;

  desc.add_options()("ip,i", value<string>()->default_value("localhost"),
                     "Server address")(
      "port,p", value<string>()->default_value("2061"),
      "Server port")("player-name,n", value<string>()->required())(
      "player-team,t", value<string>())("window-width,w", value<int>())(
      "window-height,h", value<int>());

  cout << "Toogashada Client" << endl;

  try {

    store(command_line_parser(argc, argv).options(desc).run(), vm);
    notify(vm);

    Client client{
        vm["ip"].as<string>(),
        vm["port"].as<string>(),
        vm["player-name"].as<string>(),
        vm.count("player-team") ? vm["player-team"].as<string>() : "",
        vm.count("window-width") ? vm["window-width"].as<int>() : -1,
        vm.count("window-height") ? vm["window-height"].as<int>() : -1};
    client.run();

  } catch (exception &e) {

    cerr << "Error: \n" << e.what() << endl << endl;

    cerr << desc << endl;
    return 1;
  }
}
