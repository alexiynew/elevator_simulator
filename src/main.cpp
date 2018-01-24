#include "commands.hpp"
#include "elevator.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    settings setup(argc, argv);
    bool enabled = true;

    elevator lift(setup);

    interface ui;

    ui.add_command_handler<command::call>([&lift](command::call command) {
        lift.call_to(command.floor);
    });

    ui.add_command_handler<command::move>([&lift](command::move command) {
        lift.move_to(command.floor);
    });

    ui.add_command_handler<command::exit>([&enabled, &ui](command::exit command) {
        ui.show_message("bye!");
        enabled = false;
    });

    ui.add_command_handler<command::help>([&ui](command::help command) {
        ui.show_message("help");
    });

    lift.set_message_handler([&ui](const std::string& message) {
        ui.show_message(message);
    });

    ui.show_message("=== Welcome to elevator sim 2018 ===");

    lift.run();
    while (enabled) {
        ui.process_input();

        std::this_thread::sleep_for(500ms);
    }

    return 0;
}