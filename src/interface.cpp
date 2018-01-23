#include "interface.hpp"
#include "commands.hpp"
#include <iostream>
#include <sstream>
#include <vector>

int to_int(const std::string& string) {
    std::stringstream temp(string);
    int value = 0;
    temp >> value;
    return value;
}

std::vector<std::string> split(const std::string &string, char delim) {
    std::vector<std::string> elements;

    std::stringstream temp(string);
    std::string item;
    while (std::getline(temp, item, delim)) {
        elements.push_back(item);
    }

    return elements;
}

void interface::show_message(const std::string& message)
{
	std::cout << message << std::endl;
}

void interface::process_input()
{
    std::string input;
    std::getline(std::cin, input);

    auto strings = split(input, ' ');

    const std::string command = strings.size() >= 1 ? strings[0] : "";
    const std::string argument = strings.size () >= 2 ? strings[1] : "";

    if (command == "move" || command == "m") {
        if (!argument.empty()) {
            m_dispatcher.send_command(command::move{to_int(argument)});
        } else {
            show_message("argument required");
        }   
    } else if (command == "call" || command == "c") {
        if (!argument.empty()) {
            m_dispatcher.send_command(command::call{to_int(argument)});
        } else {
            show_message("argument required");
        }   
    } else if (command == "help" || command == "h") {
    	m_dispatcher.send_command(command::help{});
    } else if (command == "exit" || command == "e") {
    	m_dispatcher.send_command(command::exit{});
    } else {
    	show_message("unknown command");
    }
}