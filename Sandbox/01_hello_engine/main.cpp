#include "EmreGL.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main()
{

    EmreGL::PrintInfo();
    const auto texturePath =
        std::filesystem::path(ENGINE_ASSETS_DIR) / "test" / "deneme.txt";
    std::cout << "Asset path: " << texturePath.string() << '\n';

    ifstream iff(texturePath.string());
	if (!iff.is_open())
	{
		std::cerr << "Failed to open file: " << texturePath.string() << '\n';
		return 1;
	}
    std::string line;
    while (getline(iff, line))
    {
        std::cout << line << '\n';
    }

    return 0;
}
