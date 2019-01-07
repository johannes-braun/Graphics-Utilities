#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

std::string make_name(std::string path)
{
    std::cout << path << '\n'; 
    return path;
}

int main()
{
    shader_dll = LoadLibraryA("vk_and_gl-shaders.dll");

    auto shader = load_shader("shaders/gl_fs.frag");

    std::cout << shader->size() << '\n';

    std::cin.ignore();
    return 0;
}
