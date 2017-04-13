#include <iostream>

#if 0
// Standard
#include <stdexcept>
#include <memory>
#include <string>
#include <thread>

// Boost
#include <boost/asio.hpp>
#endif
// Project
#include "Client.h"

using namespace std;

int main()
{
	cout << "Client" << endl;
	Client client;
	client.run();
}

