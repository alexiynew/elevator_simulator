#include "elevator.hpp"

elevator::elevator(const settings& setup)
	: m_setup(setup)
{}

elevator::~elevator()
{

}

void elevator::run()
{
	send_message("elevator::run");
}

void elevator::move_to(int floor)
{

}

void elevator::call_to(int floor)
{

}

void elevator::set_message_handler(message_handler handler)
{
	m_message_handler = std::move(handler);
}

void elevator::send_message(const std::string& message) 
{
	if (m_message_handler) {
		m_message_handler(message);
	}
}