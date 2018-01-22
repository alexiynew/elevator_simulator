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

    std::cout << "hello" << std::endl;
    std::cout << setup << std::endl;

    return 0;
}