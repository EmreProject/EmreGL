#include "EmreGL.h"

#include <filesystem>
#include <iostream>

int main()
{
    EmreGL::PrintInfo();
    const auto texturePath =
        std::filesystem::path(ENGINE_ASSETS_DIR) / "textures" / "wood.png";
    std::cout << "Asset path: " << texturePath.string() << '\n';
    return 0;
}
