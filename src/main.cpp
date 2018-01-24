#include "commands.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "elevator.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    settings setup(argc, argv);
    bool enabled = true;


    interface ui;

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


    elevator lift(setup);

    lift.set_message_handler([&ui](const std::string& message){
        ui.show_message(message);
    });


    ui.show_message("=== Welcome to elevator sim 2018 ===");

    lift.run();
    while (enabled) {
        ui.process_input();
    }

    return 0;
}