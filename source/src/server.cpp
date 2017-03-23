#include <iostream>

using namespace std;

void server(int port);

int main()
{
	cout << "Server\n";
	try {
		server(2048 + 13);
	} catch (std::exception& e)	{
		std::cerr << e.what() << std::endl;
	}
}


