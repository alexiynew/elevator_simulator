#ifndef COMMAND_DISPATCHER_HPP
#define COMMAND_DISPATCHER_HPP

#include <typeinfo>
#include <unordered_map>
#include <memory>
#include <functional>

namespace command {

class dispatcher {
public:
    template <typename T>
    void add_handler(std::function<void(T)> handler)
    {
        m_handlers[typeid(T).hash_code()] = std::make_unique<wrapped_handler<T>>(std::move(handler));
    }

	template <typename T>
    void send_command(T command)
    {
        const auto id = typeid(T).hash_code();
        if (m_handlers.count(id) && m_handlers[id]) {
            auto* handler = static_cast<wrapped_handler<T>*>(m_handlers[id].get());
            handler->function(std::forward<T>(command));
        }
    }

private:
    struct base_wrapper {
    };

    template <typename F>
    struct wrapper : public base_wrapper {
        wrapper(F function)
            : function(std::move(function)){};
        F function;
    };

    template <typename T>
    using wrapped_handler = wrapper<std::function<void(T)>>;

    std::unordered_map<size_t, std::unique_ptr<base_wrapper>> m_handlers; 
};

}

#endif