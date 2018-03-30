#include "host.hpp"
#include "states.hpp"
#include "splash.hpp"
#include <random>

bool menu() {
    float col[] = { 0, 0, 0, 1 };
    glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);

    ImGui::Begin("Lel");
    if (ImGui::Button("Run."))
        game::host::set_state(game::loop_ingame);
    if (ImGui::Button("Splash!"))
        game::host::set_state(game::prepare_splash);
    if (ImGui::Button("Quit."))
        game::host::set_state(game::do_quit);
    ImGui::End();

    return true;
}

bool ingame() {
    //static ui u(*game::host::window);
    float col[] = { 1, 0.5f, 0.2f, 1 };
    glClearNamedFramebufferfv(gl_framebuffer_t::zero, GL_COLOR, 0, col);

    ImGui::Begin("Luhl");
    if (ImGui::Button("Menu."))
        game::host::set_state(game::loop_menu);
    if (ImGui::Button("Quit."))
        game::host::set_state(game::do_quit);
    ImGui::DragFloat("Prog", &game::progress, 0.1f, 0.f, 1.f);

    ImGui::End();

    int w, h; glfwGetFramebufferSize(*game::host::window, &w, &h);

    /* u.draw_quad({0, 0 }, { w, 24.f }, {64, 64, 64, 255});
     u.draw_quad({0, 0 }, { progress * w, 24.f }, { 255, 255, 255, 255 });

     u.draw();*/
    return true;
}

#include <functional>
#include <list>

enum eval_op
{
    /*** priority: 3 ***/
    op_neg = 0, op_pos = 1, 
    op_not = 2, op_inv = 3,

    /*** priority: 5 ***/
    op_mul = 4, op_div = 5, 
    op_mod = 6,

    /*** priority: 6 ***/
    op_add = 7, op_sub = 8,

    /*** priority: 8 ***/
    op_lt = 9, op_leq = 10,
    op_gt = 11, op_geq = 12,

    /*** priority: 9 ***/
    op_eq = 13, op_neq = 14,

    /*** priority: 10 ***/
    op_and = 15,

    /*** priority: 11 ***/
    op_xor = 16,

    /*** priority: 12 ***/
    op_or = 17,

    /*** priority: 13 ***/
    op_land = 18,

    /*** priority: 14 ***/
    op_lor = 19,
};

bool xeq(const char* x, const char* y, int len)
{
    return memcmp(x, y, len) == 0;
}

struct eval_item
{
    const char* fch_at;
    const char* fch_opt_at;
    eval_op o;
};

int eval_operator(const char* beg, const eval_item& o, int len);
int eval(const char* x, int len)
{
    enum state
    {
        def = 0,
        inop = 1,
    };

    state s = def;
    std::list<eval_item> opstack;

    while (*x == ' ' || *x == '\t')
    {
        ++x;
        --len;
    }
    while (*(x + len - 1) == ' ' || *(x + len - 1) == '\t')
        --len;

    if (*x == '(' && *(x + len - 1) == ')')
        ++x, len -= 2;

    const char* begin = x;

    for (auto c = x; c < x + len; ++c)
    {
        if (s == def)
        {
            if (xeq(c, "-", 1))      opstack.push_back({ c + 1, c, op_neg });
            else if (xeq(c, "+", 1)) opstack.push_back({ c + 1, c, op_pos });
            else if (xeq(c, "!", 1)) opstack.push_back({ c + 1, c, op_not });
            else if (xeq(c, "~", 1)) opstack.push_back({ c + 1, c, op_inv });
            else if (xeq(c, "(", 1))
            {
                for (int stk = 1; stk != 0; )
                {
                    ++c;
                    if (c - begin > len) assert(false);
                    if (*c == '(') ++stk;
                    if (*c == ')') --stk;
                }
                ++c;
                s = inop;
            }
        }
        else if (s == inop)
        {
            if (xeq(c, "*", 1))       opstack.push_back({ c + 1, c, op_mul });
            else if (xeq(c, "/",  1)) opstack.push_back({ c + 1, c, op_div });
            else if (xeq(c, "%",  1)) opstack.push_back({ c + 1, c, op_mod });
            else if (xeq(c, "+",  1)) opstack.push_back({ c + 1, c, op_add });
            else if (xeq(c, "-",  1)) opstack.push_back({ c + 1, c, op_sub });
            else if (xeq(c, "<=", 2)) opstack.push_back({ c + 2, c, op_leq });
            else if (xeq(c, "<",  1)) opstack.push_back({ c + 1, c, op_lt });
            else if (xeq(c, ">=", 2)) opstack.push_back({ c + 2, c, op_geq });
            else if (xeq(c, ">",  1)) opstack.push_back({ c + 1, c, op_gt });
            else if (xeq(c, "==", 2)) opstack.push_back({ c + 2, c, op_eq });
            else if (xeq(c, "!=", 2)) opstack.push_back({ c + 2, c, op_neq });
            else if (xeq(c, "&&", 2)) opstack.push_back({ c + 2, c, op_land });
            else if (xeq(c, "||", 2)) opstack.push_back({ c + 2, c, op_lor });
            else if (xeq(c, "&",  1)) opstack.push_back({ c + 1, c, op_and });
            else if (xeq(c, "^",  1)) opstack.push_back({ c + 1, c, op_xor });
            else if (xeq(c, "|",  1)) opstack.push_back({ c + 1, c, op_or });
        }

        if (isdigit(*c))
            s = inop;
        else if (*c != ' ')
            s = def;
    }

    if (opstack.empty())
        return std::stoi(std::string(begin, begin + len));

    eval_op ox = opstack.front().o;
    auto limit = opstack.begin();
    for (auto it = opstack.begin(); it != opstack.end(); ++it)
        if (it->o > ox)
        {
            ox = it->o;
            limit = it;
        }

    if (limit == opstack.end())
        limit = opstack.begin();

    return eval_operator(begin, *limit, len);
}

int eval_operator(const char* beg, const eval_item& o, int len)
{
    const int f_count = o.fch_opt_at - beg;
    const int s_count = len - (o.fch_at - beg);

    switch (o.o)
    {
    case op_neg:    return - eval(o.fch_at, len - (o.fch_at - beg));
    case op_pos:    return + eval(o.fch_at, len - (o.fch_at - beg));
    case op_not:    return ! eval(o.fch_at, len - (o.fch_at - beg));
    case op_inv:    return ~ eval(o.fch_at, len - (o.fch_at - beg));
    case op_mul:    return eval(beg, f_count) *  eval(o.fch_at, s_count);
    case op_div:    return eval(beg, f_count) /  eval(o.fch_at, s_count);
    case op_mod:    return eval(beg, f_count) %  eval(o.fch_at, s_count);
    case op_add:    return eval(beg, f_count) +  eval(o.fch_at, s_count);
    case op_sub:    return eval(beg, f_count) -  eval(o.fch_at, s_count);
    case op_lt:     return eval(beg, f_count) <  eval(o.fch_at, s_count);
    case op_leq:    return eval(beg, f_count) <= eval(o.fch_at, s_count);
    case op_gt:     return eval(beg, f_count) >  eval(o.fch_at, s_count);
    case op_geq:    return eval(beg, f_count) >= eval(o.fch_at, s_count);
    case op_eq:     return eval(beg, f_count) == eval(o.fch_at, s_count);
    case op_neq:    return eval(beg, f_count) != eval(o.fch_at, s_count);
    case op_and:    return eval(beg, f_count) &  eval(o.fch_at, s_count);
    case op_xor:    return eval(beg, f_count) ^  eval(o.fch_at, s_count);
    case op_or:     return eval(beg, f_count) |  eval(o.fch_at, s_count);
    case op_land:   return eval(beg, f_count) && eval(o.fch_at, s_count);
    case op_lor:    return eval(beg, f_count) || eval(o.fch_at, s_count);
    }
}

int main()
{
    std::string ev = "192 - 192 || (29 * (-11+8)) == -87";

    int val = eval(ev.data(), ev.length());


    if (!game::host::init("../asset/settings.xml"))
        return 1;
    gl::shader test("simple_pt/trace.comp");

    return 0;

    game::splash splash(game::host::window);

    double d = glfwGetTime();
    while (glfwGetTime() - d < 0.5f)
    {
    }
    splash.set_progress(0.3f);
    d = glfwGetTime();
    while (glfwGetTime() - d < 0.8f)
    {
    }
    splash.set_progress(0.8f);
    d = glfwGetTime();
    while (glfwGetTime() - d < 0.4f)
    {
    }
    splash.set_progress(1.f);
    /* game::host::state_loops[game::loop_menu] = &menu;
     game::host::state_loops[game::loop_ingame] = &ingame;
     game::host::state_loops[game::prepare_splash] = &game::splash;
     game::host::state_loops[game::loop_splash] = &game::run_splash;
     game::host::set_state(game::prepare_splash);

     game::progress = 0.f;
     game::host::do_loop();
     game::host::do_loop();
     game::host::do_loop();
     std::mt19937 gn;
     std::uniform_real_distribution<float> dst(0.f, 1.f);
     while (game::progress != 1.f)
     {
         double d = glfwGetTime();
         while (glfwGetTime() - d < 0.5f)
         {
         }
         game::progress = std::min(game::progress+dst(gn), 1.f);
         game::host::do_loop();
         game::host::do_loop();
     }
     game::host::set_state(game::loop_menu);
     while (game::host::do_loop()) { }*/

    return 0;
}