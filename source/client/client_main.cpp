#include <iostream>
#include <boost/program_options.hpp>
#include <string.h>

// Project
#include "Client.h"

using namespace std;

namespace {
    const size_t ERROR_IN_COMMAND_LINE = 1;
}

int main(int argc, const char** argv)
{
    cout << "Client" << endl;

    namespace po = boost::program_options;

    boost::program_options::options_description desc("Options");
    boost::program_options::variables_map vm;

    desc.add_options()
            ("ip", po::value<std::string>()->required(), "Server address")
            ("port,p", po::value<std::string>()->required(), "Server port")
            ("player-name,pn", po::value<std::string>()->required())
            ("player-team,pt", po::value<std::string>());

    try {

        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        Client client {vm["ip"].as<std::string>(),
                       vm["port"].as<std::string>(),
                       vm["player-name"].as<std::string>(),
                       vm.count("player-team") ? vm["player-team"].as<std::string>() : ""};
        client.run();

    } catch(po::error& e) {

        std::cerr << "ERROR: "
                  << e.what()
                  << std::endl
                  << std::endl;

        std::cerr << desc << std::endl;
        return ERROR_IN_COMMAND_LINE;
    }
}

