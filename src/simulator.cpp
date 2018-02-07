#include <ostream>
#include "simulator.hpp"
#include "commands.hpp"
#include "utils.hpp"

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


simulator::simulator(const settings& setup)
    : m_setup(setup)
    , m_elevator(setup)
{
    // ui callbacks
    m_ui.add_command_handler<command::call>([this](command::call command) {
        m_elevator.call_to(command.floor);
    });

    m_ui.add_command_handler<command::move>([this](command::move command) {
        m_elevator.move_to(command.floor);
    });

    m_ui.add_command_handler<command::help>([this](command::help) {
        m_ui.show_message(help);
    });

    m_ui.add_command_handler<command::setup>([this](command::setup) {
        m_ui.show_message(utils::to_string(m_setup));
    });

    m_ui.add_command_handler<command::exit>([this](command::exit) {
        m_ui.show_message("bye!");
        m_enabled = false;
    });

    // elevator callbacks
    m_elevator.set_message_handler([this](const std::string& message) {
        m_ui.show_message(message);
    });
}

void simulator::run()
{
    m_enabled = true;
    m_ui.show_message("=== Welcome to elevator sim 2018 ===");

    m_elevator.run();

    while (m_enabled) {
        m_ui.process_input();
        std::this_thread::sleep_for(sleep_time);
    }
}
    
