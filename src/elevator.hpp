#ifndef ELEVATOR_HPP
#define ELEVATOR_HPP

#include "settings.hpp"
#include <functional>
#include <queue>
#include <string>

class elevator {
public:
	using message_handler = std::function<void(const std::string&)>;
	elevator(const settings& setup);
	~elevator();

	void run();

	void move_to(int floor);
	void call_to(int floor);

	void set_message_handler(message_handler handler);

private:
	settings m_setup;

	message_handler m_message_handler;

	int m_current_floor = 1;
	int m_target_floor = 1;

	void send_message(const std::string& message);
};

#endif