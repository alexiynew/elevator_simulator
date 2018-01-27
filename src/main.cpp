#include "settings.hpp"
#include "simulator.hpp"

int main(int argc, char* argv[])
{
    const settings setup(argc, argv);
    simulator elevator_simulator(setup);

    elevator_simulator.run();

    return 0;
}