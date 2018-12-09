#include <GLFW/glfw3.h>
#include <gfx.core/worker.hpp>
#include <gfx.ecs/ecs.hpp>
#include <thread>

#include "gfx.core/log.hpp"
#include "globals.hpp"

#include "app.hpp"


std::thread run_vulkan(ecs_state_t& ecs_state);
std::thread run_opengl(ecs_state_t& ecs_state);

//struct my_app : gfx::basic_app
//{
//    enum
//    {
//        tid_ui,
//        tid_graphics
//     };
//
//    my_app()
//    {
//        glfwInit();
//        add_thread(tid_ui, &my_app::run_ui);
//        add_thread(tid_graphics, &my_app::run_graphics);
//    }
//
//    void run_ui()
//    {
//        GLFWwindow* win = glfwCreateWindow(100, 200, "Test__", nullptr, nullptr);
//        gfx::timed_while::run([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
//            gfx::ilog << "run_ui";
//            glfwPollEvents();
//            return self.value_after(!glfwWindowShouldClose(win), 10ms);
//        });
//        hint_close(true);
//    }
//
//    void run_graphics()
//    {
//        gfx::timed_while::run([this](gfx::timed_while& self, gfx::timed_while::duration delta) {
//            gfx::ilog << "run_graphics";
//            return self.value_after(!should_close(), 5ms);
//        });
//    }
//};

int main(int argc, char** argv)
{
    glfwInit();
  /*  my_app runner11;
    my_app runner22;*/

    ecs_state_t ecs_state;

    std::vector<std::thread> wait_for;
    wait_for.emplace_back(run_opengl(ecs_state));
    wait_for.emplace_back(run_vulkan(ecs_state));

    gfx::worker input_worker([&](gfx::timed_while& self, gfx::timed_while::duration delta) {
        ecs_state.ecs.update(delta, ecs_state.inputs_list);
        return self.value_after(true, update_time_inputs);
    });

    for (auto& t : wait_for)
        if (t.joinable()) t.join();/**/
}
