#include "interface.hpp"
#include "commands.hpp"

void interface::show_message(const std::string& message)
{
}

void interface::run()
{
    send_command(command::help{});
}