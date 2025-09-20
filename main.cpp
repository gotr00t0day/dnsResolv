#include <iostream>
#include <string>
#include "modules/resolveWeb.h"

	
int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage: ./dnsResolv links.txt" << "\n";
	}
	std::string links = argv[1];
	Resolve(links);
	return 0;
}