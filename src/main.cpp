#include "commands.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const settings setup)
{
    os << "Setup:\n"
       << "\t floors count    [" << setup.floors_count() << "]\n"
       << "\t floor height    [" << setup.floors_count() << "m]\n"
       << "\t speed           [" << setup.speed() << "m/s]\n"
       << "\t doors open time [" << setup.open_time() << "s]\n";

    return os;
}

int main(int argc, char* argv[])
{
    settings setup(argc, argv);
    interface ui;

    bool enabled = true;

    ui.add_command_handler<command::call>([](command::call command) {
        std::cout << "call to " << command.floor << std::endl;
    });

    ui.add_command_handler<command::move>([](command::move command) {
        std::cout << "move to " << command.floor << std::endl;
    });

    ui.add_command_handler<command::exit>([&enabled, &ui](command::exit command) {
        ui.show_message("bye!");
        enabled = false;
    });

    ui.add_command_handler<command::help>([&ui](command::help command) {
        ui.show_message("help");
    });

    ui.show_message("=== Welcome to elevator sim 2018 ===");
    while (enabled) {
        ui.process_input();
    }

    return 0;
}