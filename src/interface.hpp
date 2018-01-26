#ifndef INTREFACE_HPP
#define INTREFACE_HPP

#include "command_dispatcher.hpp"
#include <functional>
#include <string>
#include <vector>

class interface {
public:
    interface();
    ~interface();

    void show_message(const std::string& message);

    void process_input();

    template <typename T>
    void add_command_handler(std::function<void(T)> handler)
    {
        m_dispatcher.add_handler<T>(std::move(handler));
    }

private:
    class command_parser;

    std::vector<command_parser> m_parsers;
    command::dispatcher m_dispatcher;

    void init_parsers();
};

#endif