#ifndef INTREFACE_HPP
#define INTREFACE_HPP

#include <functional>
#include <string>
#include "command_dispatcher.hpp"

class interface {
public:
    void show_message(const std::string& message);

    void process_input();

    template <typename T>
    void add_command_handler(std::function<void(T)> handler)
    {
        m_dispatcher.add_handler<T>(std::move(handler));
    }

private:
    command::dispatcher m_dispatcher;
};

#endif