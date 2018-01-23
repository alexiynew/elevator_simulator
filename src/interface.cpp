#include "interface.hpp"
#include "commands.hpp"

void interface::show_message(const std::string& message)
{
}

void interface::run()
{
    m_dispatcher.send_command(command::help{});
}