#include "elevator.hpp"
#include <cassert>
#include <chrono>
#include <sstream>

using namespace std::chrono_literals;

namespace {
std::string to_string(int value)
{
    std::stringstream temp;
    temp << value;
    return temp.str();
}
}

elevator::elevator(const settings& setup)
    : m_setup(setup)
    , m_requested_floors(setup.floors_count(), false)
{
}

elevator::~elevator()
{
    stop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void elevator::run()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_working = true;

    m_thread = std::thread(&elevator::process_requests, this);
}

void elevator::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::fill(m_requested_floors.begin(), m_requested_floors.end(), false);
    m_working = false;
}

void elevator::move_to(int floor)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    add_request(floor);
}

void elevator::call_to(int floor)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    add_request(floor);
}

void elevator::set_message_handler(message_handler handler)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_message_handler = std::move(handler);
}

void elevator::process_requests()
{
    while (m_working) {
        switch (m_state) {
        case state::waiting: {
            if (int next_floor = get_next_floor()) {
                m_target_floor = next_floor;
                assert(is_available_floor(m_target_floor));
                if (m_target_floor != m_current_floor) {
                    m_state = state::moving;
                } else {
                    m_state = state::opening_doors;
                }
            }
            break;
        }
        case state::moving: {
            if (m_current_floor == m_target_floor) {
                m_state = state::opening_doors;
            } else {
                m_current_floor = m_current_floor > m_target_floor ? m_current_floor - 1 : m_current_floor + 1;
                assert(is_available_floor(m_current_floor));
                send_message(std::string("floor ") + to_string(m_current_floor));
            }
            break;
        }
        case state::opening_doors: {
            // start timer
            send_message("open doors");
            m_state = state::waiting_doors;
            break;
        }
        case state::waiting_doors: {
            m_state = state::closing_doors;
            break;
        }
        case state::closing_doors: {
            // stop timer
            send_message("close doors");
            m_state = state::waiting;
            break;
        }
        }

        std::this_thread::sleep_for(1s);
    }
}

void elevator::send_message(const std::string& message) const
{
    if (m_message_handler) {
        m_message_handler(message);
    }
}

void elevator::add_request(int floor)
{
    if (is_available_floor(floor)) {
        m_requested_floors[floor - 1] = true;
    } else {
        send_message("selected floor is unavailable");
    }
}

bool elevator::is_available_floor(int floor) const
{
    return floor > 0 && floor <= m_setup.floors_count();
}

int elevator::get_next_floor()
{
    auto next_upper_floor = [this]() {
        for (int i = m_current_floor - 1; i < m_requested_floors.size(); ++i) {
            if (m_requested_floors[i]) {
                return i + 1;
            }
        }
    };

    auto next_lower_floor = [this]() {
        for (int i = m_current_floor - 1; i >= 0; --i) {
            if (m_requested_floors[i]) {
                return i + 1;
            }
        }
    };

    std::lock_guard<std::mutex> lock(m_mutex);

    int floor = 0;
    switch (m_direction) {
    case direction::up:
        floor = next_upper_floor();
        break;
    case direction::down:
        floor = next_lower_floor();
        break;
    case direction::none:
        floor = next_upper_floor();
        if (!floor) {
            floor = next_lower_floor();
        }
        break;
    }

    return floor;
}
