#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "settings.hpp"
#include "elevator.hpp"
#include "interface.hpp"

class simulator 
{
public:
    simulator(const settings& setup);
    void run();
    
private:
    settings m_setup;
    elevator m_elevator;
    interface m_ui;
    bool m_enabled = false;
};

#endif

