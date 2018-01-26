#include "elevator.hpp"
#include "utils.hpp"
#include <cassert>
#include <chrono>

using namespace std::chrono_literals;

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
    add_request(floor);
}

void elevator::call_to(int floor)
{
    add_request(floor);
}

void elevator::set_message_handler(message_handler handler)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_message_handler = std::move(handler);
}

void elevator::process_requests()
{
    auto select_next_floor = [this]() {
        if (int next_floor = get_next_floor()) {
            m_target_floor = next_floor;
            assert(is_available_floor(m_target_floor));
            if (m_target_floor != m_current_floor) {
                m_direction = m_target_floor > m_current_floor ? direction::up : direction::down;
                m_state = state::moving;
            } else {
                m_state = state::opening_doors;
            }
            return true;
        }
        return false;
    };

    while (m_working) {
        switch (m_state) {
        case state::waiting: {
            select_next_floor();
            break;
        }
        case state::moving: {
            if (int next_floor = get_next_floor()) {
                m_target_floor = next_floor;
            }

            if (m_current_floor == m_target_floor) {
                m_state = state::opening_doors;
                break;
            }

            if (int next_floor = get_next_floor()) {
                m_target_floor =
            } else {
                m_current_floor = m_current_floor > m_target_floor ? m_current_floor - 1 : m_current_floor + 1;
                assert(is_available_floor(m_current_floor));
                send_message(std::string("floor ") + utils::to_string(m_current_floor));
            }
            break;
        }
        case state::opening_doors: {
            // start timer
            assert(m_current_floor == m_target_floor);
            send_message("open doors");
            m_state = state::waiting_doors;
            break;
        }
        case state::waiting_doors: {
            assert(m_current_floor == m_target_floor);
            m_state = state::closing_doors;
            break;
        }
        case state::closing_doors: {
            // stop timer
            assert(m_current_floor == m_target_floor);

            send_message("close doors");
            complete_request(m_current_floor);

            if (select_next_floor()) {
                break;
            }

            m_direction = direction::none;
            if (select_next_floor()) {
                break;
            }

            m_state = state::waiting;
            break;
        }
        }

        std::this_thread::sleep_for(1s);
    }
}

void elevator::send_message(const std::string& message)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_message_handler) {
        m_message_handler(message);
    }
}

void elevator::add_request(int floor)
{
    if (is_available_floor(floor)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_requested_floors[floor - 1] = true;
    } else {
        send_message("selected floor is unavailable");
    }
}

void elevator::complete_request(int floor)
{
    assert(is_available_floor(floor));
    std::lock_guard<std::mutex> lock(m_mutex);
    m_requested_floors[floor - 1] = false;
}

bool elevator::is_available_floor(int floor) const
{
    return floor > 0 && floor <= m_setup.floors_count();
}

int elevator::get_next_floor()
{
    auto next_upper_floor = [this]() -> int {
        for (size_t i = m_current_floor - 1; i < m_requested_floors.size(); ++i) {
            if (m_requested_floors[i]) {
                return i + 1;
            }
        }
        return 0;
    };

    auto next_lower_floor = [this]() -> int {
        for (size_t i = m_current_floor - 1; i >= 0; --i) {
            if (m_requested_floors[i]) {
                return i + 1;
            }
        }
        return 0;
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
