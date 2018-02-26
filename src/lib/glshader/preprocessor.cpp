#include "preprocessor.hpp"
#include "config.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stack>
#include <cstring>

namespace glshader::preprocessor
{
    namespace fs = std::experimental::filesystem;
    std::set<std::string> extensions;

    namespace glsl_impl
    {
        void syntax_error(const fs::path& file, int line, const std::string& reason);

        bool is_new_line(const char* in);
        bool is_comment_begin(const char* in);
        bool is_space(const char* in);
        const char* skip_space(const char* in);
        const char* skip_space_reverse(const char* in);
        const char* skip_to_next_space(const char* in);
        const char* skip_to_line_end(const char* in);
        bool is_token_same(const char* text_ptr, const char* token, unsigned size, bool test_before = true, bool test_after = true);
        template<unsigned N> bool is_token_same(const char* text_ptr, const char(&token)[N], bool test_before = true, bool test_after = true);
        bool is_directive_begin(const char* text_ptr, bool test_before = true);
        const char* skip_to_next_token(const char* in);
        const char* skip_to_next_space_or(const char* in, char alt);
        std::string line_directive(const fs::path& file, int line);
        bool is_defined(const std::string& val, const processed_file& processed);
        void increment_line(int& current_line, processed_file& processed);
        const char* ignore_comments(const char* text_ptr, int& current_line, processed_file& processed, const fs::path& current_file, std::stringstream& result);
        bool is_macro(const char* text_ptr, processed_file& processed);
        std::string expand_macro(const std::string& name, const char* param_start, int param_length, const fs::path& current_file, int current_line, processed_file& processed);
        bool is_important_arithmetic_op(const char* c);
        const char* skip_prefixes(const char* c);
        const char* skip_eval_token(const char* current_character, int max_length, const fs::path& current_file, int current_line);
        int evaluate(const char* ptr, int length, const fs::path& current_file, int current_line);
        int evaluate_token(const char* ptr, int length, const fs::path& current_file, int current_line);
        const char* skip_important_operator(const char* ptr, int length, const fs::path& current_file, int current_line);
        int evaluate(const char* ptr, int length, const fs::path& current_file, int current_line);
        std::string expand_inline_macros(const char* text_ptr, const char* & text_ptr_after, const fs::path& current_file, int current_line, processed_file& processed);
        void process_impl(const fs::path& file_path, const std::vector<fs::path>& include_directories,
            processed_file& processed, std::set<fs::path>& unique_includes, std::stringstream& result);
    }

    namespace
    {
        int neg(const int in) { return -in; }
        int pos(const int in) { return in; }
        int inv(const int in) { return ~in; }
        int no(const int in) { return !in; }

        int add(const int in, const int ot) { return in + ot; }
        int sub(const int in, const int ot) { return in - ot; }
        int mul(const int in, const int ot) { return in * ot; }
        int dv(const int in, const int ot) { return in / ot; }
        int mod(const int in, const int ot) { return in % ot; }

        int op_and(const int in, const int ot) { return in & ot; }
        int op_or(const int in, const int ot) { return in | ot; }
        int op_xor(const int in, const int ot) { return in ^ ot; }
    }

    std::map<char, int(*)(int)> prefixes =
    {
        { '-', &neg },
        { '+', &pos },
        { '~', &inv },
        { '!', &no },
    };

    std::map<char, int(*)(int, int)> operators_arithmetic =
    {
        { '-', &sub },
        { '+', &add },
        { '*', &mul },
        { '/', &dv },
        { '%', &mod },
    };

    std::map<char, int(*)(int, int)> operators_logic =
    {
        { '&', &op_and },
        { '|', &op_or },
        { '^', &op_xor }
    };

    void glsl_impl::syntax_error(const fs::path& file, const int line, const std::string& reason)
    {
        std::cerr << "Error in " + file.string() + ":" + std::to_string(line) + ": " + reason << "\n";
    }

    bool glsl_impl::is_new_line(const char* in)
    {
        return *in == '\n' || *in == '\r';
    }

    bool glsl_impl::is_comment_begin(const char* in)
    {
        return (memcmp(in, "//", 2) == 0) || (memcmp(in, "/*", 2) == 0);
    }

    bool glsl_impl::is_space(const char* in)
    {
        return *in == ' ' || *in == '\t';
    }

    const char* glsl_impl::skip_space(const char* in)
    {
        while (is_space(in))
            ++in;
        return in;
    }

    const char* glsl_impl::skip_space_reverse(const char* in)
    {
        while (is_space(in))
            --in;
        return in;
    }

    const char* glsl_impl::skip_to_next_space(const char* in)
    {
        while (!is_space(in) && !is_new_line(in) && *in != '\0')
            ++in;
        return in;
    }

    const char* glsl_impl::skip_to_next_space_or(const char* in, const char alt)
    {
        while (!is_space(in) && !is_new_line(in) && *in != '\0' && *in != alt)
            ++in;
        return in;
    }

    const char* glsl_impl::skip_to_line_end(const char* in)
    {
        while (!is_new_line(in) && *in != '\0')
            ++in;
        return in;
    }

    bool glsl_impl::is_token_same(const char* text_ptr, const char* token, const unsigned size, const bool test_before,
        const bool test_after)
    {
        // N is the size of the token including the null-termination char.
        return (!test_before || (!isalpha(*(text_ptr - 1)))) &&
            (memcmp(text_ptr, token, size - 1) == 0) &&
            (!test_after || (!isalnum(*(text_ptr + size - 1)) && *(text_ptr + size - 1) != '_'));
    }

    template <unsigned N>
    bool glsl_impl::is_token_same(const char* text_ptr, const char(&token)[N], const bool test_before,
        const bool test_after)
    {
        // N is the size of the token including the null-termination char.
        return is_token_same(text_ptr, token, N, test_before, test_after);
    }

    bool glsl_impl::is_directive_begin(const char* text_ptr, const bool test_before)
    {
        // detects a directive. It's when a '#' is the first non-space symbol of a line.
        return *text_ptr == '#' && (!test_before || *skip_space_reverse(text_ptr - 1) == '\n');
    }

    const char* glsl_impl::skip_to_next_token(const char* in)
    {
        return skip_space(skip_to_next_space(in));
    }

    std::string glsl_impl::line_directive(const fs::path& file, const int line)
    {
        return "\n#line " + std::to_string(line) + " \"" + file.filename().string() + "\"\n";
    }

    bool glsl_impl::is_defined(const std::string& val, const processed_file& processed)
    {
        return processed.definitions.count(val) != 0 || (memcmp(val.data(), "GL_", 3) == 0 && extensions.count(val) != 0
            );
    }

    void glsl_impl::increment_line(int& current_line, processed_file& processed)
    {
        processed.definitions["__LINE__"] = { {}, std::to_string(++current_line) };
    }

    const char* glsl_impl::ignore_comments(const char* text_ptr, int& current_line, processed_file& processed,
        const fs::path& current_file, std::stringstream& result)
    {
        if (memcmp(text_ptr, "//", 2) == 0)
        {
            while (!is_new_line(text_ptr))
                ++text_ptr;
        }
        else if (memcmp(text_ptr, "/*", 2) == 0)
        {
            while (memcmp(text_ptr, "*/", 2) != 0)
            {
                if (is_new_line(text_ptr))
                    increment_line(current_line, processed);

                ++text_ptr;
            }

            text_ptr += 2;
            if (processed.version != -1)
            {
                result << line_directive(current_file, current_line);
            }
        }
        return text_ptr;
    }

    bool glsl_impl::is_macro(const char* text_ptr, processed_file& processed)
    {
        const auto begin = text_ptr;
        while (isalnum(*text_ptr) || *text_ptr == '_')
            ++text_ptr;

        const std::string str(begin, text_ptr);
        return is_defined(str, processed);
    }

    std::string glsl_impl::expand_macro(const std::string& name, const char* param_start, const int param_length,
        const fs::path& current_file, const int current_line, processed_file& processed)
    {
        std::stringstream stream;
        if (processed.definitions.count(name) == 0)
            return name;

        auto info = processed.definitions.at(name);

        if (info.parameters.empty())
            return info.replacement;

        if (memcmp(name.data(), "GL_", 3) == 0 && extensions.count(name) != 0)
            return "1";

        std::vector<std::string> inputs;

        if (param_start != nullptr)
        {
            std::stringstream param_stream({ param_start, param_start + param_length });
            std::string in;

            while (std::getline(param_stream, in, ','))
            {
                const auto bg = skip_space(in.data());
                inputs.push_back(in.data() + (bg - in.data()));
            }
        }

        if (inputs.size() != info.parameters.size() || (info.parameters.size() >= inputs.size() - 1 && inputs.back() ==
            "..."))
            syntax_error(current_file, current_line, "Macro " + name + ": non-matching argument count.");


        bool skip_stream = false;
        for (int replacement_offset = 0; replacement_offset < static_cast<int>(info.replacement.length()); ++
            replacement_offset)
        {
            for (int parameter = 0; parameter < static_cast<int>(info.parameters.size()); ++parameter)
            {
                if (is_token_same(&info.replacement[replacement_offset], info.parameters[parameter].data(),
                    static_cast<unsigned>(info.parameters[parameter].length() + 1),
                    replacement_offset != 0))
                {
                    skip_stream = true;
                    stream << inputs[parameter];
                    replacement_offset += static_cast<int>(info.parameters[parameter].length() - 1);
                    break;
                }
                if (is_token_same(&info.replacement[replacement_offset], "__VA_ARGS__", replacement_offset != 0) && info
                    .parameters[parameter] == "...")
                {
                    skip_stream = true;
                    for (auto input_parameter = parameter; input_parameter != inputs.size(); ++input_parameter)
                        stream << inputs[input_parameter];
                    break;
                }
            }
            if (skip_stream)
                skip_stream = false;
            else
                stream << info.replacement[replacement_offset];
        }
        return stream.str();
    }

    bool glsl_impl::is_important_arithmetic_op(const char* c)
    {
        return *c == '*' || *c == '/' || *c == '%';
    }

    const char* glsl_impl::skip_prefixes(const char* c)
    {
        while (prefixes.count(*c) != 0)
            ++c;
        return c;
    }

    const char* glsl_impl::skip_eval_token(const char* current_character, const int max_length,
        const fs::path& current_file, const int current_line)
    {
        const auto begin = current_character = skip_prefixes(skip_space(current_character));
        if (*begin == '(')
        {
            int bracket_stack = 0;
            while (true)
            {
                if (*current_character == ')')
                    --bracket_stack;
                else if (*current_character == '(')
                    ++bracket_stack;
                ++current_character;

                if (!bracket_stack)
                    break;

                if (*current_character == '\0')
                    syntax_error(current_file, current_line,
                        "Unexpected end of brackets. " + std::to_string(bracket_stack));
            }
        }
        else
        {
            while ((isalnum(*current_character) || *current_character == '_') && current_character - begin < max_length)
                ++current_character;
        }

        return current_character;
    }

    int glsl_impl::evaluate_token(const char* ptr, int length, const fs::path& current_file, const int current_line)
    {
        const auto space_skipped = skip_space(ptr);
        length = static_cast<int>(1 + length - (space_skipped - ptr));
        ptr = space_skipped;

        if (*ptr == '(')
        {
            return evaluate(ptr + 1, length - 2, current_file, current_line);
        }

        if (prefixes.count(*ptr))
        {
            const auto prefix_skipped = skip_space(ptr + 1);
            return prefixes[*ptr](evaluate_token(prefix_skipped,
                static_cast<int>(skip_eval_token(
                    prefix_skipped, static_cast<int>(length - (prefix_skipped - ptr)),
                    current_file, current_line) - prefix_skipped - 1),
                current_file, current_line));
        }

        const auto token_skipped = skip_eval_token(ptr, length, current_file, current_line);

        // range-based atoi
        int val = 0;
        for (auto p = ptr; p != token_skipped; ++p)
        {
            val = (10 * val) + (*p - '0');
        }
        return val;
    }

    const char* glsl_impl::skip_important_operator(const char* ptr, int length, const fs::path& current_file,
        const int current_line)
    {
        while (is_important_arithmetic_op(ptr) || isalnum(*ptr) || *ptr == '_' || is_space(ptr) || *ptr == '(')
        {
            const auto last = ptr;
            ptr = skip_eval_token(ptr, length, current_file, current_line);
            auto a = ptr - last;
            if (a == 0)
            {
                // Safety check to prohibit infinite loops
                a = 1;
                ++ptr;
            }
            length -= static_cast<int>(a);
        }
        return ptr;
    }

    int glsl_impl::evaluate(const char* ptr, const int length, const fs::path& current_file, const int current_line)
    {
        const auto start = ptr;
        const auto token_skipped = skip_eval_token(prefixes.count(*ptr) ? ptr + 1 : ptr, length, current_file,
            current_line);
        int current_value = evaluate_token(ptr, static_cast<int>(token_skipped - ptr - 1), current_file, current_line);
        ptr = token_skipped;

        while (*ptr != '\0' && ptr - start < length)
        {
            if (operators_arithmetic.count(*ptr) != 0)
            {
                if (is_important_arithmetic_op(ptr))
                {
                    const auto next_token_skipped = skip_eval_token(
                        ptr + 1, static_cast<int>(length - (ptr + 1 - start)), current_file, current_line);
                    const auto next_evaluated = evaluate_token(
                        ptr + 1, static_cast<int>(next_token_skipped - (ptr + 1)), current_file, current_line);
                    current_value = operators_arithmetic[*ptr](current_value, next_evaluated);
                    ptr = next_token_skipped;
                }
                else
                {
                    const auto next_token_skipped = skip_important_operator(
                        ptr + 1, static_cast<int>(length - (ptr + 1 - start)), current_file, current_line);
                    const auto next_evaluated = evaluate(ptr + 1, static_cast<int>(next_token_skipped - (ptr + 1)),
                        current_file, current_line);
                    current_value = operators_arithmetic[*ptr](current_value, next_evaluated);
                    ptr = next_token_skipped;
                }
            }
            else
            {
                ++ptr;
            }
            ptr = skip_space(ptr);
        }

        return current_value;
    }

    std::string glsl_impl::expand_inline_macros(const char* text_ptr, const char*& text_ptr_after,
        const fs::path& current_file, const int current_line,
        processed_file& processed)
    {
        std::string line(text_ptr, skip_to_line_end(text_ptr));
        bool first_replacement = true;
        while (true)
        {
            const auto begin = text_ptr;
            while (isalnum(*text_ptr) || *text_ptr == '_')
                ++text_ptr;

            const auto begin_params = skip_space(text_ptr);
            auto end_params = begin_params - 1;
            if (*begin_params == '(')
            {
                while (*end_params != ')')
                    ++end_params;
            }

            if (!is_macro(begin, processed))
            {
                if (*text_ptr == '\0' || *text_ptr == '\n' || line.empty() || text_ptr == &line[line.size() - 1])
                    break;
                ++text_ptr;
                if (*text_ptr == '\0' || *text_ptr == '\n' || line.empty() || text_ptr == &line[line.size() - 1])
                    break;
                continue;
            }

            const auto params_start = *begin_params == '(' ? begin_params + 1 : nullptr;
            const auto params_length = *begin_params == '(' ? end_params - params_start : 0;

            std::string expanded_macro = expand_macro({ begin, text_ptr }, params_start, static_cast<int>(params_length),
                current_file, current_line, processed);

            if (first_replacement)
            {
                first_replacement = false;
                text_ptr_after = end_params;
                line = expanded_macro;
            }
            else
            {
                line.replace(line.begin() + static_cast<size_t>(begin - line.data()),
                    line.begin() + static_cast<size_t>(text_ptr - line.data()), expanded_macro.begin(),
                    expanded_macro.end());
            }

            text_ptr = line.data();
            bool enable_test_macro = true;
            while (!line.empty() && ((!enable_test_macro || !is_macro(text_ptr, processed)) && text_ptr != &line[line.
                size() - 1]))
            {
                if (!(isalnum(*text_ptr) || *text_ptr == '_'))
                    enable_test_macro = true;
                else
                    enable_test_macro = false;
                ++text_ptr;
            }

            if (line.empty() || text_ptr == &line[line.size() - 1])
                break;
        }
        return line;
    }

    void glsl_impl::process_impl(const fs::path& file_path, const std::vector<fs::path>& include_directories,
        processed_file& processed, std::set<fs::path>& unique_includes,
        std::stringstream& result)
    {
        int defines_nesting = 0;
        std::stack<bool> accept_else_directive;

        std::ifstream root_file(file_path, std::ios::in);
        std::string contents(std::istreambuf_iterator<char>{root_file}, std::istreambuf_iterator<char>{});

        const char* text_ptr = contents.data();

        fs::path current_file = file_path;
        processed.definitions["__FILE__"] = { {}, current_file.string() };
        int current_line = 1;

        // There is no way you could put a macro starting from the first character of the shader.
        // Set to true if the current text_ptr may point to the start of a macro name.
        bool enable_macro = false;

        while (*text_ptr != '\0')
        {
            text_ptr = ignore_comments(text_ptr, current_line, processed, current_file, result);
            if (*text_ptr == '\0')
                break;

            if (is_new_line(text_ptr))
            {
                increment_line(current_line, processed);
                result << '\n';
                ++text_ptr;
                enable_macro = true;
            }
            else if (enable_macro && is_macro(text_ptr, processed))
            {
                result << expand_inline_macros(text_ptr, text_ptr, current_file, current_line, processed);
                ++text_ptr;
            }
            else if (is_directive_begin(text_ptr, current_line != 1))
            {
                const auto directive_name = skip_space(text_ptr + 1);
                if (is_token_same(directive_name, "version"))
                {
                    text_ptr = skip_to_next_token(directive_name);
                    processed.version = (*text_ptr - '0') * 100 +
                        (*(text_ptr + 1) - '0') * 10 +
                        (*(text_ptr + 2) - '0');

                    processed.definitions["__VERSION__"] = { {}, {text_ptr, text_ptr + 3} };

                    result << "#version " << *text_ptr << *(text_ptr + 1) << *(text_ptr + 2) << " ";
                    text_ptr = skip_to_next_token(text_ptr);

                    if (is_new_line(text_ptr))
                    {
                        processed.definitions["GL_core_profile"] = { {}, "1" };
                        processed.profile = shader_profile::core;
                    }
                    else if (is_token_same(text_ptr, "core"))
                    {
                        processed.definitions["GL_core_profile"] = { {}, "1" };
                        processed.profile = shader_profile::core;
                    }
                    else if (is_token_same(text_ptr, "compatibility"))
                    {
                        processed.definitions["GL_compatibility_profile"] = { {}, "1" };
                        processed.profile = shader_profile::compat;
                    }
                    else
                    {
                        syntax_error(current_file, current_line,
                            "Unrecognized profile: " + std::string(text_ptr, skip_to_line_end(text_ptr)));
                    }

                    while (!is_new_line(text_ptr))
                        result << *text_ptr++;

                    result << line_directive(current_file, current_line);
                }
                else if (is_token_same(directive_name, "extension"))
                {
                    text_ptr = skip_to_next_token(directive_name);
                    result << "#extension ";

                    const auto name_end = skip_to_next_space_or(text_ptr, ':');
                    const std::string extension(text_ptr, name_end);

                    result << extension << " : ";

                    text_ptr = skip_space((*(text_ptr - 1) == ':') ? text_ptr : skip_space(name_end) + 1);

                    if (is_token_same(text_ptr, "require"))
                        processed.extensions[extension] = true;
                    else if (is_token_same(text_ptr, "enable"))
                        processed.extensions[extension] = false;
                    else
                    {
                        syntax_error(current_file, current_line,
                            "Unrecognized extension requirement: " + std::string(
                                text_ptr, skip_to_line_end(text_ptr)));
                    }

                    while (!is_new_line(text_ptr))
                        result << *text_ptr++;
                }
                else if (is_token_same(directive_name, "pragma"))
                {
                    text_ptr = skip_to_next_token(directive_name);

                    if (is_token_same(text_ptr, "once"))
                    {
                        unique_includes.emplace(current_file);
                        text_ptr = skip_to_line_end(text_ptr);
                    }
                    else
                    {
                        result << "#pragma ";
                    }
                    // It is possible to add custom pragmas
                }
                else if (is_token_same(directive_name, "define"))
                {
                    text_ptr = skip_to_next_token(directive_name);
                    const auto name_begin = text_ptr;
                    while (!is_space(text_ptr) && !is_new_line(text_ptr) && *text_ptr != '(')
                        ++text_ptr;

                    if (const auto space_skipped = skip_space(text_ptr);
                    is_space(text_ptr) && !is_new_line(space_skipped) && !is_comment_begin(space_skipped))
                    {
                        // macro without params
                        auto value_end = space_skipped;
                        std::stringstream val;
                        while (!is_new_line(value_end) && *value_end != '\0' && !is_comment_begin(value_end))
                        {
                            if (*value_end != '\\')
                                val << *value_end;
                            else
                                value_end = skip_to_line_end(value_end) + 1;
                            if (is_new_line(value_end))
                                increment_line(current_line, processed);
                            ++value_end;
                        }

                        processed.definitions[{name_begin, text_ptr}] = definition_info{ val.str() };

                        text_ptr = value_end;
                    }
                    else if (is_new_line(text_ptr) || is_new_line(space_skipped) || is_comment_begin(space_skipped))
                    {
                        // define without value
                        processed.definitions[{name_begin, text_ptr}];
                    }
                    else if (*space_skipped == '(')
                    {
                        // macro with params
                        const auto name_end = text_ptr;
                        const auto params_begin = space_skipped + 1;
                        while (*text_ptr != ')')
                            ++text_ptr;
                        const auto params_end = text_ptr;

                        // macro without params
                        auto value_end = skip_space(params_end + 1);
                        std::stringstream definition_stream;
                        while (!(is_new_line(value_end) && *(value_end - 1) != '\\'))
                        {
                            if (*value_end != '\\')
                                definition_stream << *value_end;
                            if (is_new_line(value_end))
                                increment_line(current_line, processed);
                            ++value_end;
                        }

                        std::string parameter;
                        std::vector<std::string> parameters;
                        std::stringstream param_stream({ skip_space(params_begin), params_end });

                        while (std::getline(param_stream, parameter, ','))
                        {
                            const auto param_begin = skip_space(parameter.data());
                            parameters.push_back({ param_begin, skip_to_next_space(param_begin) });
                        }

                        while (param_stream >> parameter)
                        {
                            parameters.push_back(parameter);

                            if (param_stream.peek() == ',' || param_stream.peek() == ' ')
                                param_stream.ignore();
                        }

                        processed.definitions[{name_begin, name_end}] =
                        { std::move(parameters), definition_stream.str() };

                        text_ptr = value_end;
                    }

                    result << line_directive(current_file, current_line);
                }
                else if (is_token_same(directive_name, "undef"))
                {
                    text_ptr = skip_to_next_token(directive_name);
                    const auto begin = text_ptr;
                    while (*text_ptr != '\0' && !is_space(text_ptr) && !is_new_line(text_ptr))
                        ++text_ptr;

                    processed.definitions.erase({ begin, text_ptr });
                }
                else if (const auto elif = is_token_same(directive_name, "elif"); is_token_same(
                    directive_name, "if", true, false) || (elif))
                {
                    ++defines_nesting;
                    text_ptr = skip_to_next_token(directive_name);
                    const auto value_begin = text_ptr;
                    while (!is_new_line(text_ptr) && !is_space(text_ptr))
                        ++text_ptr;

                    bool evaluated;
                    if (is_token_same(directive_name, "ifdef"))
                        evaluated = is_defined({ value_begin, text_ptr }, processed);
                    else if (is_token_same(directive_name, "ifndef"))
                        evaluated = !is_defined({ value_begin, text_ptr }, processed);
                    else if (elif && !accept_else_directive.top())
                        evaluated = false;
                    else
                    {
                        // Simple IF
                        std::stringstream line;
                        for (auto i = value_begin; i != text_ptr; ++i)
                        {
                            if (memcmp(i, "//", 2) == 0)
                            {
                                break;
                            }
                            if (memcmp(i, "/*", 2) == 0)
                            {
                                while (memcmp(i, "*/", 2) != 0)
                                    ++i;
                                ++i;
                            }
                            else if (is_token_same(i, "defined"))
                            {
                                while (*i != '(')
                                    ++i;
                                const auto defined_macro_begin = i;
                                while (*i != ')')
                                    ++i;

                                line << (is_defined(std::string(defined_macro_begin + 1, i), processed) ? '1' : '0');
                            }
                            else
                                line << *i;
                        }

                        auto line_str = line.str();
                        auto str = expand_inline_macros(line_str.c_str(), text_ptr, current_file, current_line,
                            processed);

                        evaluated = evaluate(str.data(), static_cast<int>(str.length()), current_file, current_line);
                    }

                    if (evaluated)
                    {
                        text_ptr = skip_to_line_end(text_ptr);
                        if (elif)
                            accept_else_directive.top() = false;
                        else
                            accept_else_directive.push(false);
                    }
                    else
                    {
                        if (!elif)
                            accept_else_directive.push(true);
                        for (;; ++text_ptr)
                        {
                            text_ptr = ignore_comments(text_ptr, current_line, processed, current_file, result);
                            if (is_new_line(text_ptr))
                            {
                                increment_line(current_line, processed);
                            }
                            else if (const auto space_skipped = skip_space(text_ptr); (*(text_ptr - 1) == '\n') && *
                                space_skipped == '#')
                            {
                                text_ptr = space_skipped;
                                if (memcmp(skip_space(text_ptr + 1), "if", 2) == 0)
                                {
                                    auto deeper_skipped = skip_space(text_ptr);
                                    while (!(is_directive_begin(deeper_skipped) && (is_token_same(
                                        skip_space(deeper_skipped + 1), "endif") || is_token_same(
                                            deeper_skipped + 1, "elif"))))
                                    {
                                        text_ptr = ignore_comments(text_ptr, current_line, processed, current_file,
                                            result);
                                        if (is_new_line(text_ptr))
                                            increment_line(current_line, processed);
                                        ++text_ptr;
                                        deeper_skipped = skip_space(text_ptr);
                                    }
                                    text_ptr = skip_to_line_end(text_ptr);
                                }
                                else if (is_token_same(skip_space(text_ptr + 1), "elif") ||
                                    is_token_same(skip_space(text_ptr + 1), "endif") ||
                                    is_token_same(skip_space(text_ptr + 1), "else"))
                                {
                                    break;
                                }
                            }
                            else if (*text_ptr == '\0')
                            {
                                syntax_error(current_file, current_line, "no closing endif or else found.");
                                break;
                            }
                        }
                    }
                }
                else if (is_token_same(directive_name, "else"))
                {
                    if (accept_else_directive.top())
                    {
                        text_ptr = skip_to_line_end(text_ptr);
                    }
                    else
                    {
                        int nesting = 0;
                        while (true)
                        {
                            if (is_new_line(text_ptr))
                            {
                                increment_line(current_line, processed);
                            }
                            else if (is_directive_begin(text_ptr))
                            {
                                if (is_token_same(skip_space(text_ptr + 1), "if", true, false))
                                    ++nesting;
                                else if (is_token_same(skip_space(text_ptr + 1), "endif"))
                                {
                                    if (nesting == 0)
                                        break;
                                    else nesting--;
                                }
                            }
                            else if (*text_ptr == '\0')
                            {
                                syntax_error(current_file, current_line, "no closing endif found.");
                                break;
                            }

                            ++text_ptr;
                        }
                    }
                }
                else if (is_token_same(directive_name, "endif"))
                {
                    accept_else_directive.pop();
                    text_ptr = skip_to_line_end(directive_name);
                    --defines_nesting;
                    result << line_directive(current_file, current_line);
                }
                else if (is_token_same(directive_name, "line"))
                {
                    text_ptr = skip_to_next_token(directive_name);
                    const auto line_nr_end = skip_to_next_space(text_ptr);

                    result << "#line ";

                    int new_line_number = 0;
                    for (auto i = text_ptr; (i != line_nr_end) && (new_line_number *= 10) != -1; ++i)
                        new_line_number += *i - '0';

                    result << new_line_number << " ";

                    text_ptr = skip_space(line_nr_end);
                    if (*text_ptr == '\"')
                    {
                        if (const auto file_name_end = skip_to_next_space(text_ptr) - 1; *file_name_end == '\"')
                        {
                            current_file = fs::path(std::string(text_ptr + 1, file_name_end));
                            processed.definitions["__FILE__"] = { {}, current_file.string() };
                            result << "\"" << current_file << "\"";
                        }
                        else
                        {
                            syntax_error(current_file, current_line,
                                "Invalid line directive, did not find closing \".");
                        }
                    }
                    text_ptr = skip_to_line_end(text_ptr);

                    increment_line(current_line = new_line_number - 1, processed);
                }
                else if (is_token_same(directive_name, "error"))
                {
                    const auto begin = skip_to_next_token(directive_name);
                    syntax_error(current_file, current_line, std::string(begin, skip_to_line_end(begin)));
                }
                else if (is_token_same(directive_name, "include"))
                {
                    auto include_begin = skip_to_next_token(text_ptr);
                    auto include_filename = expand_inline_macros(include_begin, include_begin, current_file,
                        current_line, processed);

                    if ((include_filename.front() != '\"' && include_filename.back() != '\"') && (include_filename.
                        front() != '<' && include_filename.back() != '>'))
                    {
                        syntax_error(current_file, current_line, "Include must be in \"...\" or <...>");
                    }
                    fs::path file = { std::string(include_filename.begin() + 1, include_filename.end() - 1) };

                    bool found_file = false;
                    for (auto&& directory : include_directories)
                    {
                        if (exists(directory / file))
                        {
                            found_file = true;
                            file = directory / file;
                        }
                    }

                    if (!found_file)
                    {
                        file = file_path.parent_path() / file;
                    }

                    if (!fs::exists(file))
                        syntax_error(current_file, current_line, "File not found: " + file.string());

                    if (unique_includes.count(file) == 0)
                    {
                        result << line_directive(file, 1);
                        process_impl(file, include_directories, processed, unique_includes, result);
                        processed.dependencies.emplace(file);
                    }
                    text_ptr = skip_to_line_end(include_begin);

                    result << line_directive(current_file, current_line);
                }
                else
                {
                    if (!(isalpha(*text_ptr) || *text_ptr == '_'))
                        enable_macro = true;
                    else
                        enable_macro = false;
                    result << *text_ptr;
                    ++text_ptr;
                }
            }
            else
            {
                if (!(isalpha(*text_ptr) || *text_ptr == '_'))
                    enable_macro = true;
                else
                    enable_macro = false;

                result << *text_ptr;
                ++text_ptr;
            }
        }
    }

    processed_file preprocess_file(const fs::path& file_path, const std::vector<fs::path>& include_directories,
        const std::vector<definition>& definitions)
    {
#if defined(glGetIntegerv) && defined(GL_NUM_EXTENSIONS) && defined(glGetStringi) && defined(GL_EXTENSIONS)
        [[maybe_unused]] const static auto ext = [] {
            int n;
            glGetIntegerv(GL_NUM_EXTENSIONS, &n);
            for (auto i = 0; i < n; ++i)
                extensions.emplace(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i)));
            return 0;
        }();
#endif

        processed_file processed;
        processed.version = -1;
        processed.file_path = file_path;

        for (auto&& definition : definitions)
            processed.definitions[definition.name] = definition.info;

        std::stringstream result;
        std::set<fs::path> unique_includes;
        unique_includes.emplace(file_path);
        glsl_impl::process_impl(file_path, include_directories, processed, unique_includes, result);

        processed.contents = result.str();
        return processed;
    }
}
