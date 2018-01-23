#ifndef INTREFACE_HPP
#define INTREFACE_HPP

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

class interface {
public:
    void show_message(const std::string& message);

    void run();

    template <typename TCommand>
    void add_command_handler(std::function<void(TCommand)> handler)
    {
        const auto& id = typeid(TCommand);
        m_handlers[id.hash_code()] = std::make_shared<concret_handler<std::function<void(TCommand)>>>(std::move(handler));
    }

private:
    struct handler {
    };

    template <typename THandler>
    struct concret_handler : public handler {
        concret_handler(THandler h)
            : handler(std::move(h)){};
        THandler handler;
    };

    std::unordered_map<size_t, std::shared_ptr<handler>> m_handlers;

    template <typename TCommand>
    void send_command(TCommand command)
    {
        const auto& id = typeid(TCommand);
        if (m_handlers.count(id.hash_code())) {
            if (auto h = m_handlers[id.hash_code()]) {
                auto* handler = reinterpret_cast<concret_handler<std::function<void(TCommand)>>*>(h.get());
                handler->handler(std::forward<TCommand>(command));
            }
        }
    }
};

#endif