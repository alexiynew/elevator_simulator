#include "interface.hpp"
#include "commands.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>

namespace {
std::vector<std::string> split(const std::string& string, char delim)
{
    std::vector<std::string> elements;

    std::stringstream temp(string);
    std::string item;
    while (std::getline(temp, item, delim)) {
        elements.push_back(item);
    }

    return elements;
}
}

class interface::command_parser {
public:
    using action_function = std::function<void(const std::string&)>;
    command_parser(const std::vector<std::string>& strings, action_function action)
        : m_strings(strings)
        , m_action(std::move(action))
    {
    }

    bool process(const std::string& command, const std::string& argument) const
    {
        if (std::find(m_strings.cbegin(), m_strings.cend(), command) != m_strings.cend()) {
            if (m_action) {
                m_action(argument);
                return true;
            }
        }
        return false;
    }

private:
    const std::vector<std::string> m_strings;
    const action_function m_action;
};

interface::interface()
{
    init_parsers();
}

interface::~interface() = default;

void interface::show_message(const std::string& message)
{
    std::cout << message << std::endl;
}

void interface::process_input()
{
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        return;
    }

    auto strings = split(input, ' ');

    const std::string command = strings.size() >= 1 ? strings[0] : "";
    const std::string argument = strings.size() >= 2 ? strings[1] : "";

    for (const auto& parser : m_parsers) {
        if (parser.process(command, argument)) {
            return;
        }
    }

    show_message("Unknown command");
}

void interface::init_parsers()
{
    auto on_move_command = [this](const std::string& argument) {
        if (!argument.empty()) {
            m_dispatcher.send_command(command::move{ utils::value_or<int>(argument, 0) });
        } else {
            show_message("Argument required");
        }
    };

    auto on_call_command = [this](const std::string& argument) {
        if (!argument.empty()) {
            m_dispatcher.send_command(command::call{ utils::value_or<int>(argument, 0) });
        } else {
            show_message("Argument required");
        }
    };

    auto on_help_command = [this](const std::string& argument) {
        m_dispatcher.send_command(command::help{});
    };

    auto on_setup_command = [this](const std::string& argument) {
        m_dispatcher.send_command(command::setup{});
    };

    auto on_exit_command = [this](const std::string& argument) {
        m_dispatcher.send_command(command::exit{});
    };

    m_parsers.push_back({ { "move", "m" }, std::move(on_move_command) });
    m_parsers.push_back({ { "call", "c" }, std::move(on_call_command) });
    m_parsers.push_back({ { "help", "h" }, std::move(on_help_command) });
    m_parsers.push_back({ { "setup", "s" }, std::move(on_setup_command) });
    m_parsers.push_back({ { "exit", "e" }, std::move(on_exit_command) });
}
