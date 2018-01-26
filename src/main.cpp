#include "commands.hpp"
#include "elevator.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "utils.hpp"
#include <ostream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

std::ostream& operator<<(std::ostream& os, const settings& setup)
{
    os << "Setup:\n"
       << "\t floors count    [" << setup.floors_count() << "]\n"
       << "\t floor height    [" << setup.floor_height() << "m]\n"
       << "\t speed           [" << setup.speed() << "m/s]\n"
       << "\t doors open time [" << setup.doors_open_time() << "s]\n";

    return os;
}

const std::string help = "Commands:\n\
\t'move N', 'm N' - move elevator to floor N, from the elevator.\n\
\t'call N', 'c N' - call elevator to floor N, from floor.\n\
\t'help', 'h'     - show this help.\n\
\t'setup', 's'    - show elevator setup.\n\
\t'exit', 'e'     - end simulation.";

const std::chrono::milliseconds sleep_time = 20ms;

int main(int argc, char* argv[])
{
    const settings setup(argc, argv);
    bool enabled = true;

    elevator lift(setup);

    interface ui;

    // ui callbacks
    ui.add_command_handler<command::call>([&lift](command::call command) {
        lift.call_to(command.floor);
    });

    ui.add_command_handler<command::move>([&lift](command::move command) {
        lift.move_to(command.floor);
    });

    ui.add_command_handler<command::help>([&ui](command::help) {
        ui.show_message(help);
    });

    ui.add_command_handler<command::setup>([&ui, &setup](command::setup) {
        ui.show_message(utils::to_string(setup));
    });

    ui.add_command_handler<command::exit>([&enabled, &ui](command::exit) {
        ui.show_message("bye!");
        enabled = false;
    });

    // elevator callbacks
    lift.set_message_handler([&ui](const std::string& message) {
        ui.show_message(message);
    });

    // start
    ui.show_message("=== Welcome to elevator sim 2018 ===");

    lift.run();

    while (enabled) {
        ui.process_input();

        std::this_thread::sleep_for(sleep_time);
    }

    lift.stop();

    return 0;
}