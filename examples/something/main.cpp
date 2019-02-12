#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <memory>
#define MYGL_IMPLEMENTATION
#include <mygl/mygl.hpp>
#include <vector>

constexpr auto vs_r2 = R"(
#version 460 core

vec2 r2(uint num)
{
    const float g = 1.32471795724474602596;
    const float a1 = 1.0 / g;
    const float a2 = 1.0 / (g*g);
    return fract(0.5 + vec2(a1, a2) * num);
}

void main()
{
    gl_Position = vec4(r2(51653 + uint(gl_VertexID)), 0.5f, 1.f);
}
)";

constexpr auto vs_ham = R"(
#version 460 core

float ri(uint num)
{
    return float(bitfieldReverse(num))*2.328306437080797375431469961868475648078246891516e-10;
}

vec2 ham(uint num, uint s)
{
    return vec2(float(num) / float(s), ri(num));
}

void main()
{
    gl_Position = vec4(ham(uint(gl_VertexID), 4000) - vec2(1, 0), 0.5f, 1.f);
}
)";

constexpr auto fs = R"(
#version 460 core

layout(location = 0) out vec4 color;

void main() { color = vec4(1.f, 0.5f, 0.f, 1.f); }
)";

int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    const auto w = glfwCreateWindow(512, 512, "Test", nullptr, nullptr);
    glfwMakeContextCurrent(w);
    mygl::load();

    const auto vert = glCreateShader(GL_VERTEX_SHADER);
    const auto vert2 = glCreateShader(GL_VERTEX_SHADER);
    const auto frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &vs_r2, nullptr);
    glShaderSource(vert2, 1, &vs_ham, nullptr);
    glShaderSource(frag, 1, &fs, nullptr);
    glCompileShader(vert);
    glCompileShader(vert2);
    glCompileShader(frag);

    int  l;
    char buf[512];
    glGetShaderInfoLog(vert2, 512, &l, buf);
    std::cout << buf << '\n';
    glGetShaderInfoLog(frag, 512, &l, buf);
    std::cout << buf << '\n';

    const auto prog = glCreateProgram();
    const auto prog2 = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    glAttachShader(prog2, vert2);
    glAttachShader(prog2, frag);
    glLinkProgram(prog2);

    glGetProgramInfoLog(prog, 512, &l, buf);
    std::cout << buf << '\n';

    mygl::vertex_array arr;
    glCreateVertexArrays(1, &arr);

    int count = 0;
    glClearColor(1, 1, 1, 1);
    while (!glfwWindowShouldClose(w))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glPointSize(4);
        count = (count + 1) % 4000;
        glBindVertexArray(arr);

        glUseProgram(prog);
        glDrawArrays(GL_POINTS, 0, count);

        glUseProgram(prog2);
        glDrawArrays(GL_POINTS, 0, count);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}